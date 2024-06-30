// Some copyright should be here...

#include "AnchorpointSourceControlProvider.h"

#include <SourceControlHelpers.h>

#include "AnchorpointControlCommand.h"
#include "AnchorpointSourceControlWorker.h"
#include "ScopedSourceControlProgress.h"

void FAnchorpointSourceControlProvider::RegisterWorker(const FName& InName, const FGetAnchorpointSourceControlWorker& InDelegate)
{
	WorkersMap.Add(InName, InDelegate);
}

void FAnchorpointSourceControlProvider::Init(bool bForceConnection)
{
}

void FAnchorpointSourceControlProvider::Close()
{
}

const FName& FAnchorpointSourceControlProvider::GetName() const
{
	static FName ProviderName("Anchorpoint");
	return ProviderName;
}

FText FAnchorpointSourceControlProvider::GetStatusText() const
{
	return INVTEXT("TODO: GetStatusText");
}

TMap<ISourceControlProvider::EStatus, FString> FAnchorpointSourceControlProvider::GetStatus() const
{
	TMap<EStatus, FString> Result;
	Result.Add(EStatus::Enabled, IsEnabled() ? TEXT("Yes") : TEXT("No"));
	Result.Add(EStatus::Connected, (IsEnabled() && IsAvailable()) ? TEXT("Yes") : TEXT("No"));

	//Result.Add(EStatus::User, UserName);
	//Result.Add(EStatus::Repository, PathToRepositoryRoot);
	//Result.Add(EStatus::Remote, RemoteUrl);
	//Result.Add(EStatus::Branch, BranchName);
	//Result.Add(EStatus::Email, UserEmail);

	//Port,
	//User,
	//Client,
	//ScmVersion,
	//PluginVersion,
	//Workspace,
	//WorkspacePath,
	//Changeset

	return Result;
}

bool FAnchorpointSourceControlProvider::IsEnabled() const
{
	return bGitRepositoryFound;
}

bool FAnchorpointSourceControlProvider::IsAvailable() const
{
	return bGitRepositoryFound;
}

bool FAnchorpointSourceControlProvider::QueryStateBranchConfig(const FString& ConfigSrc, const FString& ConfigDest)
{
	return false;
}

void FAnchorpointSourceControlProvider::RegisterStateBranches(const TArray<FString>& BranchNames, const FString& ContentRoot)
{
}

int32 FAnchorpointSourceControlProvider::GetStateBranchIndex(const FString& BranchName) const
{
	return INDEX_NONE;
}

ECommandResult::Type FAnchorpointSourceControlProvider::GetState(const TArray<FString>& InFiles, TArray<FSourceControlStateRef>& OutState, EStateCacheUsage::Type InStateCacheUsage)
{
	return ECommandResult::Succeeded;
}

ECommandResult::Type FAnchorpointSourceControlProvider::GetState(const TArray<FSourceControlChangelistRef>& InChangelists,
                                                                 TArray<FSourceControlChangelistStateRef>& OutState,
                                                                 EStateCacheUsage::Type InStateCacheUsage)
{
	return ECommandResult::Failed;
}

TArray<FSourceControlStateRef> FAnchorpointSourceControlProvider::GetCachedStateByPredicate(TFunctionRef<bool(const FSourceControlStateRef&)> Predicate) const
{
	TArray<FSourceControlStateRef> Result;
	for (const auto& CacheItem : StateCache)
	{
		/*
		 FSourceControlStateRef State = CacheItem.Value;
		 if(Predicate(State))
		 {
			 Result.Add(State);
		 }
		 */
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

ECommandResult::Type FAnchorpointSourceControlProvider::Execute(const FSourceControlOperationRef& InOperation,
                                                                FSourceControlChangelistPtr InChangelist,
                                                                const TArray<FString>& InFiles,
                                                                EConcurrency::Type InConcurrency,
                                                                const FSourceControlOperationComplete& InOperationCompleteDelegate)
{
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
	return false;
}

bool FAnchorpointSourceControlProvider::UsesUncontrolledChangelists() const
{
	return true;
}

bool FAnchorpointSourceControlProvider::UsesCheckout() const
{
	return false;
}

bool FAnchorpointSourceControlProvider::UsesFileRevisions() const
{
	return false;
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
	return TOptional<bool>();
}

TOptional<int> FAnchorpointSourceControlProvider::GetNumLocalChanges() const
{
	return TOptional<int>();
}

void FAnchorpointSourceControlProvider::OutputCommandMessages(const FAnchorpointSourceControlCommand& InCommand) const
{
	FMessageLog SourceControlLog("SourceControl");

	for(int32 ErrorIndex = 0; ErrorIndex < InCommand.ErrorMessages.Num(); ++ErrorIndex)
	{
		SourceControlLog.Error(FText::FromString(InCommand.ErrorMessages[ErrorIndex]));
	}

	for(int32 InfoIndex = 0; InfoIndex < InCommand.InfoMessages.Num(); ++InfoIndex)
	{
		SourceControlLog.Info(FText::FromString(InCommand.InfoMessages[InfoIndex]));
	}
}


void FAnchorpointSourceControlProvider::Tick()
{
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
	return SNew(STextBlock).Text(INVTEXT("Hello from Anchorpoint settings"));
}

TSharedPtr<IAnchorpointSourceControlWorker> FAnchorpointSourceControlProvider::CreateWorker(const FName& OperationName)
{
	if (const FGetAnchorpointSourceControlWorker* Operation = WorkersMap.Find(OperationName))
	{
		return Operation->Execute();
	}

	return nullptr;
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