// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointSourceControlProvider.h"

#include <SourceControlHelpers.h>
#include <ScopedSourceControlProgress.h>
#include <SourceControlOperations.h>
#include <ISourceControlModule.h>
#include <UObject/ObjectSaveContext.h>
#include <Logging/MessageLog.h>
#include <FileHelpers.h>
#include <Interfaces/IPluginManager.h>

#include "AnchorpointCliConnectSubsystem.h"
#include "AnchorpointCliOperations.h"
#include "AnchorpointLog.h"
#include "AnchorpointPopup.h"
#include "AnchorpointSourceControlCommand.h"
#include "AnchorpointSourceControlWorker.h"
#include "AnchorpointSourceControlState.h"
#include "AnchorpointSourceControlSettingsWidget.h"

FAnchorpointSourceControlProvider::FAnchorpointSourceControlProvider()
{
	UPackage::PackageSavedWithContextEvent.AddRaw(this, &FAnchorpointSourceControlProvider::HandlePackageSaved);

	if(FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetOnModalLoopTickEvent().AddRaw(this, &FAnchorpointSourceControlProvider::TickDuringModal);
	}
}

FAnchorpointSourceControlProvider::~FAnchorpointSourceControlProvider()
{
	UPackage::PackageSavedWithContextEvent.RemoveAll(this);

	if(FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetOnModalLoopTickEvent().RemoveAll(this);
	}
}

bool FAnchorpointSourceControlProvider::HasAnyConflicts() const
{
	for (const TTuple<FString, TSharedRef<FAnchorpointSourceControlState>>& CacheItem : StateCache)
	{
		FSourceControlStateRef State = CacheItem.Value;
		if (State->IsConflicted())
		{
			return true;
		}
	}

	return false;
}

void FAnchorpointSourceControlProvider::Init(bool bForceConnection)
{
}

void FAnchorpointSourceControlProvider::Close()
{
	UE_LOG(LogAnchorpoint, Display, TEXT("Anchorpoint close command received - waiting for all commands to finish"));

	int32 NumCommand = 0;

	for (const FAnchorpointSourceControlCommand* Command : CommandQueue)
	{
		if (Command)
		{
			NumCommand++;

			const FString CommandName = Command->Operation->GetName().ToString();
			UE_LOG(LogAnchorpoint, Display, TEXT("Waiting command: %s[%s/%s]"), *CommandName, *LexToString(NumCommand), *LexToString(CommandQueue.Num()));

			while (!Command->bExecuteProcessed)
			{
				FPlatformProcess::Sleep(0.01f);
			}
		}
	}

	UE_LOG(LogAnchorpoint, Warning, TEXT("Anchorpoint close command finished"));
}

const FName& FAnchorpointSourceControlProvider::GetName() const
{
	// Another hack because FEditorBuildUtils::WorldPartitionBuildNavigation doesn't wrap the SCCProvider argument in quotes.
	// Therefore, with our standard implementation it will get -SCCProvider=Anchorpoint (Git) on the command line.
	// This means, during initialization the Provider will be "Anchorpoint" and "(Git)" will be a secondary ignored argument.
	// To avoid this, while the modal is running, we will be using a SCCProvider friendly name - "Anchorpoint".
	if (ActiveModalState == EActiveModalState::WorldPartitionBuildNavigation)
	{
		static FName SCCProviderName = FName("Anchorpoint");
		return SCCProviderName;
	}
	
	static FName ProviderName = []()
	{
		// NOTE: Related to comment above, when passed via commandline, the SCCProvider might look slightly different.
		// To gracefully handle these cases, we are going to use the name provided for the provider, as long as it's Anchorpoint related.
		FString ParamProvider;
		if (FParse::Value(FCommandLine::Get(), TEXT("SCCProvider="), ParamProvider))
		{
			if (ParamProvider.Contains("Anchorpoint"))
			{
				return FName(ParamProvider);
			}
		}

		// If not SCCProvider was listed, we default to our user-friendly facing name.
		return FName("Anchorpoint (Git)");
	}();

	return ProviderName;
}

FText FAnchorpointSourceControlProvider::GetStatusText() const
{
	TArray<FString> StatusMessages;

	UAnchorpointCliConnectSubsystem* ConnectSubsystem = GEditor->GetEditorSubsystem<UAnchorpointCliConnectSubsystem>();

	const bool bCliConnected = ConnectSubsystem->IsCliConnected();
	StatusMessages.Add(FString::Printf(TEXT("CLI connected: %s"), *LexToString(bCliConnected)));

	const bool bProjectConnected = ConnectSubsystem->IsProjectConnected();
	StatusMessages.Add(FString::Printf(TEXT("Project connected: %s"), *LexToString(bProjectConnected)));

	const bool bValidCache = ConnectSubsystem->GetCachedStatus().IsSet();
	StatusMessages.Add(FString::Printf(TEXT("Status cache valid: %s"), *LexToString(bValidCache)));

	const FTimespan TimeSinceLastSync = FDateTime::Now() - GetLastSyncTime();
	const int32 TimeSeconds = FMath::FloorToInt(TimeSinceLastSync.GetTotalSeconds());
	if (TimeSeconds >= 0)
	{
		StatusMessages.Add(FString::Printf(TEXT("Last sync %d seconds ago"), TimeSeconds));
	}
	else
	{
		StatusMessages.Add(TEXT("Never synced"));
	}

	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Anchorpoint"));
	StatusMessages.Add(FString::Printf(TEXT("Plugin version: %s"), *Plugin->GetDescriptor().VersionName));

	return FText::FromString(FString::Join(StatusMessages, TEXT("\n")));
}


#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
TMap<ISourceControlProvider::EStatus, FString> FAnchorpointSourceControlProvider::GetStatus() const
{
	// ToImplement: Here we should fill out as many information points as we can

	TMap<EStatus, FString> Result;
	Result.Add(EStatus::Enabled, true ? TEXT("Yes") : TEXT("No"));
	Result.Add(EStatus::Connected, true ? TEXT("Yes") : TEXT("No"));

	//Port,
	//User,
	//Client,
	//Repository,
	//Remote,
	//Branch,
	//Email,
	//ScmVersion,
	//PluginVersion,
	//Workspace,
	//WorkspacePath,
	//Changeset

	return Result;
}
#endif

bool FAnchorpointSourceControlProvider::IsEnabled() const
{
	// As long as the plugin is installed, we want to display Anchorpoint in the provider list no matter what
	// even if the Anchorpoint Desktop App is not installed, the user should be prompted to do so during setup
	return true;
}

bool FAnchorpointSourceControlProvider::IsAvailable() const
{
	return AnchorpointCliOperations::IsInstalled();
}

bool FAnchorpointSourceControlProvider::QueryStateBranchConfig(const FString& ConfigSrc, const FString& ConfigDest)
{
	// This seems to be a Perforce specific implementation we can safely ignore
	return false;
}

void FAnchorpointSourceControlProvider::RegisterStateBranches(const TArray<FString>& BranchNames, const FString& ContentRoot)
{
	// ToImplement: Here we should specify what branches we care about checking check-out operations for.
	// For Anchorpoint specific, I think we care about all the branches, so we might be able to ignore this. Need to double-check with team.
}

int32 FAnchorpointSourceControlProvider::GetStateBranchIndex(const FString& BranchName) const
{
	// Same as RegisterStateBranches. Need to double-check with the team
	return INDEX_NONE;
}

#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 7)
bool FAnchorpointSourceControlProvider::GetStateBranchAtIndex(int32 BranchIndex, FString& OutBranchName) const
{
	// Same as RegisterStateBranches. Need to double-check with the team
	return false;
}
#endif

ECommandResult::Type FAnchorpointSourceControlProvider::GetState(const TArray<FString>& InFiles, TArray<FSourceControlStateRef>& OutState, EStateCacheUsage::Type InStateCacheUsage)
{
	if (!IsEnabled())
	{
		return ECommandResult::Failed;
	}

	TArray<FString> AbsoluteFiles = SourceControlHelpers::AbsoluteFilenames(InFiles);

	if (InStateCacheUsage == EStateCacheUsage::ForceUpdate)
	{
		ISourceControlProvider::Execute(ISourceControlOperation::Create<FUpdateStatus>(), AbsoluteFiles);
	}

	for (const FString& AbsoluteFile : AbsoluteFiles)
	{
		OutState.Add(GetStateInternal(*AbsoluteFile));
	}

	return ECommandResult::Succeeded;
}

ECommandResult::Type FAnchorpointSourceControlProvider::GetState(const TArray<FSourceControlChangelistRef>& InChangelists, TArray<FSourceControlChangelistStateRef>& OutState, EStateCacheUsage::Type InStateCacheUsage)
{
	// Git doesn't have changelists functionality, therefore we can ignore a changelist-based GetState
	return ECommandResult::Failed;
}

TArray<FSourceControlStateRef> FAnchorpointSourceControlProvider::GetCachedStateByPredicate(TFunctionRef<bool(const FSourceControlStateRef&)> Predicate) const
{
	TArray<FSourceControlStateRef> Result;
	for (const TTuple<FString, TSharedRef<FAnchorpointSourceControlState>>& CacheItem : StateCache)
	{
		FSourceControlStateRef State = CacheItem.Value;
		if (Predicate(State))
		{
			Result.Add(State);
		}
	}
	return Result;
}

FDelegateHandle FAnchorpointSourceControlProvider::RegisterSourceControlStateChanged_Handle(const FSourceControlStateChanged::FDelegate& SourceControlStateChanged)
{
	return OnSourceControlStateChanged.Add(SourceControlStateChanged);
}

void FAnchorpointSourceControlProvider::UnregisterSourceControlStateChanged_Handle(FDelegateHandle Handle)
{
	OnSourceControlStateChanged.Remove(Handle);
}

ECommandResult::Type FAnchorpointSourceControlProvider::Execute(const FSourceControlOperationRef& InOperation, FSourceControlChangelistPtr InChangelist, const TArray<FString>& InFiles, EConcurrency::Type InConcurrency, const FSourceControlOperationComplete& InOperationCompleteDelegate)
{
	// ToImplement: Everything in this function needs to be checked 

	// Only Connect operation allowed while not Enabled (Connected)
	if (!IsEnabled() && InOperation->GetName() != "Connect")
	{
		(void)InOperationCompleteDelegate.ExecuteIfBound(InOperation, ECommandResult::Failed);
		return ECommandResult::Failed;
	}

	TSharedPtr<IAnchorpointSourceControlWorker> Worker = CreateWorker(InOperation->GetName());

	// Query to see if we allow this operation
	if (!Worker.IsValid())
	{
		// this operation is unsupported by this source control provider
		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("OperationName"), FText::FromName(InOperation->GetName()));
		Arguments.Add(TEXT("ProviderName"), FText::FromName(GetName()));
		FText Message(FText::Format(INVTEXT("Operation '{OperationName}' not supported by revision control provider '{ProviderName}'"), Arguments));
		FMessageLog("SourceControl").Error(Message);
		InOperation->AddErrorMessge(Message);

		(void)InOperationCompleteDelegate.ExecuteIfBound(InOperation, ECommandResult::Failed);
		return ECommandResult::Failed;
	}

	TArray<FString> AbsoluteFiles = SourceControlHelpers::AbsoluteFilenames(InFiles);

	FAnchorpointSourceControlCommand* Command = new FAnchorpointSourceControlCommand(InOperation, Worker.ToSharedRef());
	Command->Files = AbsoluteFiles;
	Command->OperationCompleteDelegate = InOperationCompleteDelegate;

	// fire off operation
	if (InConcurrency == EConcurrency::Synchronous)
	{
		Command->bAutoDelete = false;

		const FText PromptText = GetPromptTextForOperation(InOperation);
		return ExecuteSynchronousCommand(*Command, PromptText);
	}

	Command->bAutoDelete = true;
	return IssueCommand(*Command);
}

#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)

bool FAnchorpointSourceControlProvider::CanExecuteOperation(const FSourceControlOperationRef& InOperation) const
{
	return WorkersMap.Find(InOperation->GetName()) != nullptr;
}

#endif

bool FAnchorpointSourceControlProvider::CanCancelOperation(const FSourceControlOperationRef& InOperation) const
{
	return false;
}

void FAnchorpointSourceControlProvider::CancelOperation(const FSourceControlOperationRef& InOperation)
{
}

TArray<TSharedRef<ISourceControlLabel>> FAnchorpointSourceControlProvider::GetLabels(const FString& InMatchingSpec) const
{
	TArray<TSharedRef<ISourceControlLabel>> Tags;
	return Tags;
}

TArray<FSourceControlChangelistRef> FAnchorpointSourceControlProvider::GetChangelists(EStateCacheUsage::Type InStateCacheUsage)
{
	return TArray<FSourceControlChangelistRef>();
}

bool FAnchorpointSourceControlProvider::UsesLocalReadOnlyState() const
{
	return false;
}

bool FAnchorpointSourceControlProvider::UsesChangelists() const
{
	return false;
}

bool FAnchorpointSourceControlProvider::UsesUncontrolledChangelists() const
{
	// Anchorpoint does not need uncontrolled changelists and git generally does not support them, so most of the time, we want to return false.
	// However, after reverting a writeable file, we need to make sure that the file is not added to the PackagesNotToPromptAnyMore
	// Otherwise, the user will not be prompted to checkout/make writable the file again
	// More info and context on the problem in the following link: point 3. of https://discord.com/channels/764147942298484737/1272849248005787718/1308842855317377085

	// In order to avoid this, we need `UncontrolledChangelistModule.OnMakeWritable(Filename)` to return true
	// To achieve this, use an empty call to PromptForCheckoutAndSave, if bIsPromptingForCheckoutAndSave is true, we will get a PR_Cancelled
	// This way, we can ensure we only return true when the user is actively saving a writeable file

	const FEditorFileUtils::EPromptReturnCode Code = FEditorFileUtils::PromptForCheckoutAndSave({}, false, false, nullptr, false, true);
	const bool bISaving = Code == FEditorFileUtils::EPromptReturnCode::PR_Cancelled;
	return bISaving;
}

bool FAnchorpointSourceControlProvider::UsesCheckout() const
{
	// Another hack because the SSourceControlSubmitWidget has no API to disable the "Keep files checked out"
	// Even the GitSourceControlModule lists at the top:
	// '''
	// ### Known issues:
	// standard Editor commit dialog ask if user wants to "Keep Files Checked Out" => no use for Git or Mercurial CanCheckOut()==false
	// '''
	//	
	// The Tick & TickDuringModal are actively scanning to check if the user is actively submitting
	// During such times the Anchorpoint Source Control Provider has the Checkout capabilities disabled to disable the "Keep Files Checked Out" button
	if (ActiveModalState == EActiveModalState::Submit)
	{
		return false;
	}

	return true;
}

bool FAnchorpointSourceControlProvider::UsesFileRevisions() const
{
	return true;
}

bool FAnchorpointSourceControlProvider::UsesSnapshots() const
{
	return false;
}

bool FAnchorpointSourceControlProvider::AllowsDiffAgainstDepot() const
{
	return true;
}

TOptional<bool> FAnchorpointSourceControlProvider::IsAtLatestRevision() const
{
	// ToImplement: Here we should check if the file is up to date with the latest
	return TOptional<bool>();
}

TOptional<int> FAnchorpointSourceControlProvider::GetNumLocalChanges() const
{
	// ToImplement: Here we should check have many files we changed on disk
	return TOptional<int>();
}

void FAnchorpointSourceControlProvider::Tick()
{
	ActiveModalState = EActiveModalState::None;

	// ToImplement: I am unhappy with the current async execution, maybe we can find something better 
	bool bStatesUpdated = false;
	for (int32 CommandIndex = 0; CommandIndex < CommandQueue.Num(); ++CommandIndex)
	{
		FAnchorpointSourceControlCommand& Command = *CommandQueue[CommandIndex];
		if (Command.bExecuteProcessed)
		{
			// Remove command from the queue
			CommandQueue.RemoveAt(CommandIndex);

			// let command update the states of any files
			bStatesUpdated |= Command.Worker->UpdateStates();

			// dump any messages to output log
			OutputCommandMessages(Command);

			Command.ReturnResults();

			// commands that are left in the array during a tick need to be deleted
			if (Command.bAutoDelete)
			{
				// Only delete commands that are not running 'synchronously'
				delete &Command;
			}

			// only do one command per tick loop, as we dont want concurrent modification
			// of the command queue (which can happen in the completion delegate)
			break;
		}

		Command.Worker->TickWhileInProgress();
	}

	if (bStatesUpdated)
	{
		OnStatesChanged();

		OnSourceControlStateChanged.Broadcast();
	}
}

TSharedRef<SWidget> FAnchorpointSourceControlProvider::MakeSettingsWidget() const
{
	return SNew(SAnchorpointSourceControlSettingsWidget);
}

void FAnchorpointSourceControlProvider::OnStatesChanged()
{
	// List of files we already warned the user about - once this file is spotted with a non-conflicting state,
	// it is removed from the list, so we can emit warnings again.
	static TArray<FString> AlreadyWarnedFiles;

	bool bPopupNeeded = false;
	for (const TTuple<FString, TSharedRef<FAnchorpointSourceControlState>>& Cache : StateCache)
	{
		if (Cache.Value->State == EAnchorpointState::Conflicted)
		{
			if (!AlreadyWarnedFiles.Contains(Cache.Key))
			{
				AlreadyWarnedFiles.Add(Cache.Key);

				// New conflict file detected, we will notify the user!
				bPopupNeeded = true;
			}
		}
		else
		{
			AlreadyWarnedFiles.Remove(Cache.Key);
		}
	}

	if(bPopupNeeded)
	{
		AnchorpointPopups::ShowConflictPopup();
	}
}

void FAnchorpointSourceControlProvider::TickDuringModal(float DeltaTime)
{
	if (const TSharedPtr<SWindow> ActiveModalWindow = FSlateApplication::Get().GetActiveModalWindow())
	{
		TSharedRef<SWidget> ModalContent = ActiveModalWindow->GetContent();
		if (ModalContent->GetType() == TEXT("SSourceControlSubmitWidget"))
		{
			ActiveModalState = EActiveModalState::Submit;
		}
		else if (ModalContent->GetType() == TEXT("SWorldPartitionBuildNavigationDialog"))
		{
			ActiveModalState = EActiveModalState::WorldPartitionBuildNavigation;
		}
	}
}

void FAnchorpointSourceControlProvider::HandlePackageSaved(const FString& InPackageFilename, UPackage* InPackage, FObjectPostSaveContext InObjectSaveContext)
{
	// This will automatically clear and re-add if it's already on-going
	FTimerDelegate RefreshDelegate = FTimerDelegate::CreateRaw(this, &FAnchorpointSourceControlProvider::RefreshStatus);
	GEditor->GetTimerManager()->SetTimer(RefreshTimerHandle, RefreshDelegate, RefreshDelay, false);
}

void FAnchorpointSourceControlProvider::RefreshStatus()
{
	ISourceControlModule::Get().GetProvider().Execute(ISourceControlOperation::Create<FUpdateStatus>());
}

TSharedRef<FAnchorpointSourceControlState> FAnchorpointSourceControlProvider::GetStateInternal(const FString& Filename)
{
	if (TSharedRef<FAnchorpointSourceControlState>* State = StateCache.Find(Filename))
	{
		return *State;
	}

	TSharedRef<FAnchorpointSourceControlState> NewState = MakeShared<FAnchorpointSourceControlState>(Filename);
	StateCache.Add(Filename, NewState);
	return NewState;
}

FDateTime FAnchorpointSourceControlProvider::GetLastSyncTime() const
{
	FDateTime LastSyncTime = FDateTime::MinValue();
	for (const TTuple<FString, TSharedRef<FAnchorpointSourceControlState>>& Cache : StateCache)
	{
		if (Cache.Value->TimeStamp > LastSyncTime)
		{
			LastSyncTime = Cache.Value->TimeStamp;
		}
	}

	return LastSyncTime;
}

TSharedPtr<IAnchorpointSourceControlWorker> FAnchorpointSourceControlProvider::CreateWorker(const FName& OperationName)
{
	if (const FGetAnchorpointSourceControlWorker* Operation = WorkersMap.Find(OperationName))
	{
		return Operation->Execute();
	}

	return nullptr;
}

void FAnchorpointSourceControlProvider::OutputCommandMessages(const FAnchorpointSourceControlCommand& InCommand) const
{
	FMessageLog SourceControlLog("SourceControl");

	for (int32 ErrorIndex = 0; ErrorIndex < InCommand.ErrorMessages.Num(); ++ErrorIndex)
	{
		SourceControlLog.Error(FText::FromString(InCommand.ErrorMessages[ErrorIndex]));
	}

	for (int32 InfoIndex = 0; InfoIndex < InCommand.InfoMessages.Num(); ++InfoIndex)
	{
		SourceControlLog.Info(FText::FromString(InCommand.InfoMessages[InfoIndex]));
	}
}


ECommandResult::Type FAnchorpointSourceControlProvider::ExecuteSynchronousCommand(FAnchorpointSourceControlCommand& InCommand, const FText& TaskPrompt)
{
	ECommandResult::Type Result = ECommandResult::Failed;

	// Display the progress dialog if a string was provided
	{
		// Perforce uses an empty text for the progress in FPerforceSourceControlProvider::ExecuteSynchronousCommand
		// In our case, the caller of this function may can choose to hide the prompt by sending an empty text
		FScopedSourceControlProgress Progress(TaskPrompt);

		// Issue the command asynchronously...
		IssueCommand(InCommand);

		// ... then wait for its completion (thus making it synchronous)
		while (!InCommand.bExecuteProcessed)
		{
			// Tick the command queue and update progress.
			Tick();

			Progress.Tick();

			// Sleep for a bit so we don't busy-wait so much.
			FPlatformProcess::Sleep(0.01f);
		}

		// always do one more Tick() to make sure the command queue is cleaned up.
		Tick();

		if (InCommand.bCommandSuccessful)
		{
			Result = ECommandResult::Succeeded;
		}
	}

	// Delete the command now (asynchronous commands are deleted in the Tick() method)
	check(!InCommand.bAutoDelete);

	// ensure commands that are not auto deleted do not end up in the command queue
	if (CommandQueue.Contains(&InCommand))
	{
		CommandQueue.Remove(&InCommand);
	}
	delete &InCommand;

	return Result;
}

ECommandResult::Type FAnchorpointSourceControlProvider::IssueCommand(FAnchorpointSourceControlCommand& InCommand)
{
	if (GThreadPool != nullptr)
	{
		// Queue this to our worker thread(s) for resolving
		GThreadPool->AddQueuedWork(&InCommand);
		CommandQueue.Add(&InCommand);
		return ECommandResult::Succeeded;
	}
	else
	{
		FText Message(INVTEXT("There are no threads available to process the revision control command."));

		FMessageLog("SourceControl").Error(Message);
		InCommand.bCommandSuccessful = false;
		InCommand.Operation->AddErrorMessge(Message);

		return InCommand.ReturnResults();
	}
}

FText FAnchorpointSourceControlProvider::GetPromptTextForOperation(const FSourceControlOperationRef& InOperation) const
{
	UAnchorpointCliConnectSubsystem* ConnectSubsystem = GEditor->GetEditorSubsystem<UAnchorpointCliConnectSubsystem>();
	if (!ConnectSubsystem->IsProjectConnected())
	{
		// NOTE: While the CLI is not connected, we want to display every pop-up so the user can understand why things are slower.
		return InOperation->GetInProgressString();
	}
	
	if (InOperation->GetName() == TEXT("CheckIn") || InOperation->GetName() == TEXT("Revert"))
	{
		return InOperation->GetInProgressString();
	}

	if (InOperation->GetName() == TEXT("UpdateStatus"))
	{
		TSharedRef<FUpdateStatus> Operation = StaticCastSharedRef<FUpdateStatus>(InOperation);
		if (Operation->ShouldUpdateHistory())
		{
			return NSLOCTEXT("Anchorpoint", "UpdateStatusWithHistory", "Updating file(s) Revision Control history...");
		}
	}

	return FText::GetEmpty();
}