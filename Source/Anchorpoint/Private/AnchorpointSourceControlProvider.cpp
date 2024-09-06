// Some copyright should be here...

#include "AnchorpointSourceControlProvider.h"

#include <SourceControlHelpers.h>

#include "AnchorpointCliOperations.h"
#include "AnchorpointControlCommand.h"
#include "AnchorpointSourceControlWorker.h"
#include "AnchorpointControlState.h"
#include "AnchorpointSourceControlSettingsWidget.h"
#include "ScopedSourceControlProgress.h"
#include "SourceControlOperations.h"

#define LOCTEXT_NAMESPACE "Anchorpoint"

void FAnchorpointSourceControlProvider::RegisterWorker(const FName& InName, const FGetAnchorpointSourceControlWorker& InDelegate)
{
	WorkersMap.Add(InName, InDelegate);
}

void FAnchorpointSourceControlProvider::Init(bool bForceConnection)
{
}

void FAnchorpointSourceControlProvider::Close()
{
	// ToImplement: Here we should do any deinitialization we might need.
}

const FName& FAnchorpointSourceControlProvider::GetName() const
{
	static FName ProviderName("Anchorpoint");
	return ProviderName;
}

FText FAnchorpointSourceControlProvider::GetStatusText() const
{
	const FTimespan TimeSinceLastSync = FDateTime::Now() - GetLastSyncTime();
	const int32 TimeSeconds = FMath::FloorToInt(TimeSinceLastSync.GetTotalSeconds());
	if (TimeSeconds < 0)
	{
		return INVTEXT("Not synced yet");
	}

	return FText::Format(INVTEXT("Last sync {0} seconds ago"), FText::AsNumber(TimeSeconds));
}


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
	for (const TTuple<FString, TSharedRef<FAnchorpointControlState>>& CacheItem : StateCache)
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
	if (!IsEnabled() && !(InOperation->GetName() == "Connect"))
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
		return ExecuteSynchronousCommand(*Command, InOperation->GetInProgressString());
	}

	Command->bAutoDelete = true;
	return IssueCommand(*Command);
}

bool FAnchorpointSourceControlProvider::CanExecuteOperation(const FSourceControlOperationRef& InOperation) const
{
	return WorkersMap.Find(InOperation->GetName()) != nullptr;
}

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
	return true;
}

bool FAnchorpointSourceControlProvider::UsesUncontrolledChangelists() const
{
	return false;
}

bool FAnchorpointSourceControlProvider::UsesCheckout() const
{
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
	}

	if (bStatesUpdated)
	{
		OnSourceControlStateChanged.Broadcast();
	}
}

TSharedRef<SWidget> FAnchorpointSourceControlProvider::MakeSettingsWidget() const
{
	return SNew(SAnchorpointSourceControlSettingsWidget);
}

TSharedRef<FAnchorpointControlState> FAnchorpointSourceControlProvider::GetStateInternal(const FString& Filename)
{
	if (TSharedRef<FAnchorpointControlState>* State = StateCache.Find(Filename))
	{
		return *State;
	}

	TSharedRef<FAnchorpointControlState> NewState = MakeShared<FAnchorpointControlState>(Filename);
	StateCache.Add(Filename, NewState);
	return NewState;
}

FDateTime FAnchorpointSourceControlProvider::GetLastSyncTime() const
{
	FDateTime LastSyncTime = FDateTime::MinValue();
	for (const TTuple<FString, TSharedRef<FAnchorpointControlState>>& Cache : StateCache)
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


ECommandResult::Type FAnchorpointSourceControlProvider::ExecuteSynchronousCommand(FAnchorpointSourceControlCommand& InCommand, const FText& Task)
{
	ECommandResult::Type Result = ECommandResult::Failed;

	// Display the progress dialog if a string was provided
	{
		FScopedSourceControlProgress Progress(Task);

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

#undef LOCTEXT_NAMESPACE