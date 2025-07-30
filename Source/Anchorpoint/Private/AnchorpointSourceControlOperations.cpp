// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointSourceControlOperations.h"

#include <Async/Async.h>
#include <ISourceControlModule.h>
#include <SourceControlOperations.h>

#include "Anchorpoint.h"
#include "AnchorpointCli.h"
#include "AnchorpointCliConnectSubsystem.h"
#include "AnchorpointCliOperations.h"
#include "AnchorpointLog.h"
#include "AnchorpointPopup.h"
#include "AnchorpointSourceControlCommand.h"
#include "AnchorpointSourceControlState.h"
#include "AnchorpointSourceControlProvider.h"

bool UpdateCachedStates(const TArray<FAnchorpointSourceControlState>& InStates)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(UpdateCachedStates);

	FAnchorpointSourceControlProvider& Provider = FAnchorpointModule::Get().GetProvider();

	for (const FAnchorpointSourceControlState& InState : InStates)
	{
		TSharedRef<FAnchorpointSourceControlState> State = Provider.GetStateInternal(InState.LocalFilename);
		*State = InState;
		State->TimeStamp = FDateTime::Now();
	}

	return !InStates.IsEmpty();
}

bool RunUpdateStatus(const TArray<FString>& InputPaths, TArray<FAnchorpointSourceControlState>& OutState, bool bForced)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RunUpdateStatus);

	const auto StatusResult = AnchorpointCliOperations::GetStatus(InputPaths, bForced);
	if (StatusResult.HasError())
	{
		return false;
	}

	const auto CurrentUserResult = AnchorpointCliOperations::GetCurrentUser();
	if (CurrentUserResult.HasError())
	{
		return false;
	}

	const FAnchorpointStatus Status = StatusResult.GetValue();

	for (const FString& File : Status.GetAllAffectedFiles())
	{
		FAnchorpointSourceControlState& NewState = OutState.Emplace_GetRef(File);

		const FString* LockedBy = Status.Locked.Find(File);
		const bool bIsOutdated = Status.Outdated.Contains(File);
		const bool bLockedByMe = LockedBy && *LockedBy == CurrentUserResult.GetValue();

		if (auto StagedState = Status.Staged.Find(File))
		{
			if (*StagedState == EAnchorpointFileOperation::Conflicted)
			{
				NewState.State = EAnchorpointState::Conflicted;
			}
			else if (*StagedState == EAnchorpointFileOperation::Added)
			{
				NewState.State = EAnchorpointState::Added;
			}
			else if (*StagedState == EAnchorpointFileOperation::Modified)
			{
				if (!bIsOutdated)
				{
					NewState.State = bLockedByMe ? EAnchorpointState::LockedModified : EAnchorpointState::UnlockedModified;
				}
				else
				{
					NewState.State = bLockedByMe ? EAnchorpointState::OutDatedLockedModified : EAnchorpointState::OutDatedUnlockedModified;
				}
			}
			else if (*StagedState == EAnchorpointFileOperation::Deleted)
			{
				NewState.State = bLockedByMe ? EAnchorpointState::LockedDeleted : EAnchorpointState::UnlockedDeleted;
			}
			else
			{
				NewState.State = EAnchorpointState::Unknown;
			}
		}
		else if (auto NotStagedState = Status.NotStaged.Find(File))
		{
			if (*NotStagedState == EAnchorpointFileOperation::Conflicted)
			{
				NewState.State = EAnchorpointState::Conflicted;
			}
			else if (*NotStagedState == EAnchorpointFileOperation::Added)
			{
				NewState.State = EAnchorpointState::Added;
			}
			else if (*NotStagedState == EAnchorpointFileOperation::Modified)
			{
				if (!bIsOutdated)
				{
					NewState.State = bLockedByMe ? EAnchorpointState::LockedModified : EAnchorpointState::UnlockedModified;
				}
				else
				{
					NewState.State = bLockedByMe ? EAnchorpointState::OutDatedLockedModified : EAnchorpointState::OutDatedUnlockedModified;
				}
			}
			else if (*NotStagedState == EAnchorpointFileOperation::Deleted)
			{
				NewState.State = bLockedByMe ? EAnchorpointState::LockedDeleted : EAnchorpointState::UnlockedDeleted;
			}
			else
			{
				NewState.State = EAnchorpointState::Unknown;
			}
		}
		else if (bIsOutdated)
		{
			NewState.State = EAnchorpointState::OutDated;
		}
		else if (LockedBy)
		{
			if (bLockedByMe)
			{
				NewState.State = EAnchorpointState::LockedUnchanged;
			}
			else
			{
				TValueOrError<FString, FString> LockerInfoResult = AnchorpointCliOperations::GetUserDisplayName(*LockedBy);
				const FString LockerDisplayName = LockerInfoResult.HasValue() ? LockerInfoResult.GetValue() : *LockedBy;

				NewState.State = EAnchorpointState::LockedBySomeone;
				NewState.OtherUserCheckedOut = LockerDisplayName;
			}
		}
		else
		{
			NewState.State = EAnchorpointState::UnlockedUnchanged;
		}
	}

	TArray<FString> AllRelevantFiles = InputPaths;
	if (AllRelevantFiles.IsEmpty())
	{
		// For project-wide search (not input paths), we will perform this revert on all files in the source control provider's cache
		ISourceControlProvider& Provider = ISourceControlModule::Get().GetProvider();
		FAnchorpointSourceControlProvider* AnchorpointProvider = static_cast<FAnchorpointSourceControlProvider*>(&Provider);
		AnchorpointProvider->StateCache.GenerateKeyArray(AllRelevantFiles);
	}

	// In case we have files that are not in the status, we add them as UnlockedUnchanged
	for (const FString& Path : AllRelevantFiles)
	{
		if (FPaths::FileExists(Path))
		{
			bool bEntryExists = OutState.ContainsByPredicate([&Path](const FAnchorpointSourceControlState& State) { return State.LocalFilename == Path; });
			if (!bEntryExists)
			{
				FAnchorpointSourceControlState& NewState = OutState.Emplace_GetRef(Path);
				NewState.State = EAnchorpointState::UnlockedUnchanged;
			}
		}
	}

	return true;
}

FName FAnchorpointConnectWorker::GetName() const
{
	return "Connect";
}

bool FAnchorpointConnectWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointConnectWorker::Execute);

	TValueOrError<FString, FString> CurrentUserResult = AnchorpointCliOperations::GetCurrentUser();

	if (CurrentUserResult.HasError())
	{
		InCommand.ErrorMessages.Add(CurrentUserResult.GetError());
	}

	UE_LOG(LogAnchorpoint, Display, TEXT("Disabling Anchorpoint's Auto-Lock feature"));
	AnchorpointCliOperations::DisableAutoLock();

	AsyncTask(ENamedThreads::GameThread,
	          [this]()
	          {
		          if (IConsoleVariable* EnableRevertViaOutlinerVar = IConsoleManager::Get().FindConsoleVariable(TEXT("SourceControl.Revert.EnableFromSceneOutliner")))
		          {
			          UE_LOG(LogAnchorpoint, Display, TEXT("Anchorpoint Source Control Provider is setting SourceControl.Revert.EnableFromSceneOutliner to true"));;
			          EnableRevertViaOutlinerVar->Set(true);
		          }
	          });

	FAnchorpointCliModule::Get().OnAnchorpointConnected.Broadcast();

	InCommand.bCommandSuccessful = CurrentUserResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointConnectWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointConnectWorker::UpdateStates);

	return true;
}

FName FAnchorpointCheckOutWorker::GetName() const
{
	return "CheckOut";
}

bool FAnchorpointCheckOutWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckOutWorker::Execute);

	UAnchorpointCliConnectSubsystem* ConnectSubsystem = GEditor->GetEditorSubsystem<UAnchorpointCliConnectSubsystem>();
	const FDelegateHandle PreMessageHandle = ConnectSubsystem->OnPreMessageHandled.AddRaw(this, &FAnchorpointCheckOutWorker::OnCliConnectMessage);

	TValueOrError<FString, FString> LockResult = AnchorpointCliOperations::LockFiles(InCommand.Files);

	ConnectSubsystem->OnPreMessageHandled.Remove(PreMessageHandle);

	if (LockResult.HasError())
	{
		FString LockError = LockResult.GetError();

		AsyncTask(ENamedThreads::GameThread,
		          [LockError]()
		          {
			          FText Title = NSLOCTEXT("Anchorpoint", "CheckoutError", "Unable to Check Out From Revision Control!");
			          FText Message = FText::FromString(LockError);

			          FMessageDialog::Open(EAppMsgType::Ok, Message, Title);
		          });

		InCommand.ErrorMessages.Add(LockResult.GetError());
	}

	if (LockResult.HasValue())
	{
		FAnchorpointSourceControlProvider& Provider = FAnchorpointModule::Get().GetProvider();

		for (const FString& File : InCommand.Files)
		{
			TSharedRef<FAnchorpointSourceControlState> CurrentState = Provider.GetStateInternal(File);
			if (CurrentState->State == EAnchorpointState::UnlockedDeleted)
			{
				FAnchorpointSourceControlState& State = States.Emplace_GetRef(File);
				State.State = EAnchorpointState::LockedDeleted;
			}
			else if (CurrentState->State == EAnchorpointState::UnlockedModified)
			{
				FAnchorpointSourceControlState& State = States.Emplace_GetRef(File);
				State.State = EAnchorpointState::LockedModified;
			}
			else if (CurrentState->State == EAnchorpointState::UnlockedUnchanged)
			{
				FAnchorpointSourceControlState& State = States.Emplace_GetRef(File);
				State.State = EAnchorpointState::LockedUnchanged;
			}
			else if (CurrentState->State == EAnchorpointState::OutDated
				  || CurrentState->State == EAnchorpointState::OutDatedUnlockedModified)
			{
				FAnchorpointSourceControlState& State = States.Emplace_GetRef(File);
				State.State = EAnchorpointState::OutDatedLockedModified;
			}
		}
	}

	InCommand.bCommandSuccessful = LockResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCheckOutWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckOutWorker::UpdateStates);

	return UpdateCachedStates(States);
}

void FAnchorpointCheckOutWorker::TickWhileInProgress()
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckOutWorker::TickWhileInProgress);

	const TArray<FSlowTask*>& ScopeStack = GWarn->GetScopeStack();
	for (FSlowTask* Task : ScopeStack)
	{
		const int TotalFilesToLock = Task->TotalAmountOfWork;
		const FString LockProgressMessage = FString::Printf(TEXT("Locking files... (%d/%d)"), NumFilesLockedSinceStart, TotalFilesToLock);

		Task->FrameMessage = FText::FromString(LockProgressMessage);
		Task->CompletedWork = NumFilesLockedSinceStart;
		Task->TickProgress();
	}
}

void FAnchorpointCheckOutWorker::OnCliConnectMessage(const FAnchorpointConnectMessage& Message)
{
	if (Message.Type == TEXT("files locked"))
	{
		NumFilesLockedSinceStart += Message.Files.Num();
	}
}

FName FAnchorpointRevertWorker::GetName() const
{
	return TEXT("Revert");
}

bool FAnchorpointRevertWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointRevertWorker::Execute);

	// TODO: What is soft revert and how should we use it?
	// TSharedRef<FRevert> RevertOperation = StaticCastSharedRef<FRevert>(InCommand.Operation);
	// RevertOperation->IsSoftRevert()

	TArray<FString> FilesToDelete;
	TArray<FString> FilesToRevert;

	FAnchorpointSourceControlProvider& Provider = FAnchorpointModule::Get().GetProvider();

	for (const FString& File : InCommand.Files)
	{
		TSharedRef<FAnchorpointSourceControlState> CurrentState = Provider.GetStateInternal(File);
		if (CurrentState->State == EAnchorpointState::Added)
		{
			FilesToDelete.Add(File);
		}
		else
		{
			FilesToRevert.Add(File);
		}
	}

	TValueOrError<FString, FString> DeleteResult = AnchorpointCliOperations::DeleteFiles(FilesToDelete);
	if (DeleteResult.HasError())
	{
		InCommand.ErrorMessages.Add(DeleteResult.GetError());
	}

	TValueOrError<FString, FString> RevertResult = AnchorpointCliOperations::Revert(FilesToRevert);
	if (RevertResult.HasError())
	{
		InCommand.ErrorMessages.Add(RevertResult.GetError());
	}

	InCommand.bCommandSuccessful = DeleteResult.HasValue() && RevertResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointRevertWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointRevertWorker::UpdateStates);

	return true;
}

FName FAnchorpointUpdateStatusWorker::GetName() const
{
	return "UpdateStatus";
}

bool FAnchorpointUpdateStatusWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointUpdateStatusWorker::Execute);

	TSharedRef<FUpdateStatus> Operation = StaticCastSharedRef<FUpdateStatus>(InCommand.Operation);

	InCommand.bCommandSuccessful = RunUpdateStatus(InCommand.Files, States, Operation->ShouldForceUpdate());

	if (Operation->ShouldUpdateHistory())
	{
		for (int32 Index = 0; Index < InCommand.Files.Num(); Index++)
		{
			FString& File = InCommand.Files[Index];
			TValueOrError<FAnchorpointHistory, FString> HistoryResult = AnchorpointCliOperations::GetHistoryInfo(File);

			if (HistoryResult.HasValue())
			{
				InCommand.bCommandSuccessful = true;

				TAnchorpointSourceControlHistory History;
				TArray<FAnchorpointHistoryEntry> HistoryResultValue = HistoryResult.GetValue();
				for (const FAnchorpointHistoryEntry& HistoryResultEntry : HistoryResultValue)
				{
					const int RevisionNumber = HistoryResultValue.Num() - History.Num();
					TSharedRef<FAnchorpointSourceControlRevision> HistoryItem = MakeShared<FAnchorpointSourceControlRevision>(HistoryResultEntry, File, RevisionNumber);
					History.Emplace(MoveTemp(HistoryItem));
				}

				Histories.Add(*File, History);
			}
			else
			{
				InCommand.bCommandSuccessful = false;
			}
		}
	}

	for (const FAnchorpointSourceControlState& State : States)
	{
		if (State.State == EAnchorpointState::Conflicted)
		{
			TValueOrError<FAnchorpointConflictStatus, FString> ConflictResult = AnchorpointCliOperations::GetConflictStatus(State.LocalFilename);
			Conflicts.Add(State.LocalFilename, MoveTemp(ConflictResult.GetValue()));
		}
	}

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointUpdateStatusWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointUpdateStatusWorker::UpdateStates);

	bool bUpdated = UpdateCachedStates(States);

	for (const auto& History : Histories)
	{
		TSharedRef<FAnchorpointSourceControlState> State = FAnchorpointModule::Get().GetProvider().GetStateInternal(History.Key);
		State->History = History.Value;
		State->TimeStamp = FDateTime::Now();
		bUpdated = true;
	}

	for (const TTuple<FString, FAnchorpointConflictStatus>& Conflict : Conflicts)
	{
		TSharedRef<FAnchorpointSourceControlState> State = FAnchorpointModule::Get().GetProvider().GetStateInternal(Conflict.Key);
		State->PendingResolveInfo.BaseFile = Conflict.Value.CommonAncestorFilename;
		State->PendingResolveInfo.BaseRevision = Conflict.Value.CommonAncestorFileId;
		State->PendingResolveInfo.RemoteFile = Conflict.Value.RemoteFilename;
		State->PendingResolveInfo.RemoteRevision = Conflict.Value.RemoteFileId;
		bUpdated = true;
	}

	return bUpdated;
}

FName FAnchorpointAddWorker::GetName() const
{
	return "Add";
}

bool FAnchorpointAddWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointAddWorker::Execute);

	InCommand.bCommandSuccessful = true;
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointAddWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointAddWorker::UpdateStates);

	return true;
}

FName FAnchorpointCopyWorker::GetName() const
{
	return "Copy";
}

bool FAnchorpointCopyWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCopyWorker::Execute);

	InCommand.bCommandSuccessful = true;
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCopyWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCopyWorker::UpdateStates);

	return true;
}

FName FAnchorpointDeleteWorker::GetName() const
{
	return "Delete";
}

bool FAnchorpointDeleteWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointDeleteWorker::Execute);

	TValueOrError<FString, FString> LockResult = AnchorpointCliOperations::LockFiles(InCommand.Files);

	TValueOrError<FString, FString> DeleteResult = AnchorpointCliOperations::DeleteFiles(InCommand.Files);

	if (DeleteResult.HasError())
	{
		InCommand.ErrorMessages.Add(DeleteResult.GetError());
	}

	InCommand.bCommandSuccessful = DeleteResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointDeleteWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointDeleteWorker::UpdateStates);

	return true;
}

FName FAnchorpointCheckInWorker::GetName() const
{
	return "CheckIn";
}


bool FAnchorpointCheckInWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckInWorker::Execute);

	FAnchorpointSourceControlProvider& AnchorpointProvider = FAnchorpointModule::Get().GetProvider();
	if (AnchorpointProvider.HasAnyConflicts())
	{
		AnchorpointPopups::ShowConflictPopupAnyThread();

		InCommand.ErrorMessages.Add(TEXT("Repository is in conflict state"));
		InCommand.bCommandSuccessful = false;
		return false;
	}

	//TODO: Since we are running the lock with --git & --keep should we unlock the files automatically?

	TSharedRef<FCheckIn> Operation = StaticCastSharedRef<FCheckIn>(InCommand.Operation);
	const FString Message = Operation->GetDescription().ToString();

	TValueOrError<FString, FString> SubmitResult = AnchorpointCliOperations::SubmitFiles(InCommand.Files, Message);

	if (SubmitResult.HasError())
	{
		InCommand.ErrorMessages.Add(SubmitResult.GetError());
	}
	else
	{
		Operation->SetSuccessMessage(NSLOCTEXT("Anchorpoint", "CheckInSuccess", "Commit successful. Pushing files in background."));
	}

	InCommand.bCommandSuccessful = SubmitResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCheckInWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckInWorker::UpdateStates);

	return true;
}

FName FAnchorpointDownloadFileWorker::GetName() const
{
	return "DownloadFile";
}

bool FAnchorpointDownloadFileWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointDownloadFileWorker::Execute);

	TSharedRef<FDownloadFile> Operation = StaticCastSharedRef<FDownloadFile>(InCommand.Operation);
	const FString TargetDirectory = Operation->GetTargetDirectory();

	InCommand.bCommandSuccessful = true;

	if (TargetDirectory.IsEmpty())
	{
		UE_LOG(LogAnchorpoint, Error, TEXT("In memory storage not yet supported."));
		return false;
	}
	else
	{
		// Downloading the files directly to a target directory
		for (const FString& TargetFilePath : InCommand.Files)
		{
			const FString BaseFilename = FPaths::GetBaseFilename(TargetFilePath);
			const FString TempFilePath = FPaths::CreateTempFilename(*FPaths::ProjectSavedDir(), *BaseFilename.Left(32));

			FString FilePath;
			FString Commit;
			TargetFilePath.Split(TEXT("#"), &FilePath, &Commit);

			TValueOrError<FString, FString> DownloadResult = AnchorpointCliOperations::DownloadFileViaCommitId(Commit, FilePath, TempFilePath);
			InCommand.bCommandSuccessful &= DownloadResult.HasValue();

			const FString FinalTargetPath = TargetDirectory / FPaths::GetCleanFilename(TargetFilePath);
			const bool bMoveSuccess = IFileManager::Get().Move(*FinalTargetPath, *TempFilePath);
			InCommand.bCommandSuccessful &= bMoveSuccess;
		}
	}

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointDownloadFileWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointDownloadFileWorker::UpdateStates);

	// Downloading a file from the server will never affect the cached file states.
	return false;
}

FName FAnchorpointResolveWorker::GetName() const
{
	return "Resolve";
}

bool FAnchorpointResolveWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointResolveWorker::Execute);

	TValueOrError<FString, FString> ResolveResult = AnchorpointCliOperations::MarkConflictSolved(InCommand.Files);

	if (ResolveResult.HasError())
	{
		InCommand.ErrorMessages.Add(ResolveResult.GetError());
	}

	InCommand.bCommandSuccessful = ResolveResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointResolveWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointResolveWorker::UpdateStates);

	for (const FString& Filename : UpdatedFiles)
	{
		TSharedRef<FAnchorpointSourceControlState> State = FAnchorpointModule::Get().GetProvider().GetStateInternal(Filename);
		State->PendingResolveInfo = {};
	}

	return UpdatedFiles.Num() > 0;
}