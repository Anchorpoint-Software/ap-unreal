// Some copyright should be here...

#include "AnchorpointSouceControlOperations.h"

#include <SourceControlOperations.h>

#include "Anchorpoint.h"
#include "AnchorpointCliOperations.h"
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

bool RunUpdateStatus(const TArray<FString>& InputPaths, TArray<FAnchorpointSourceControlState>& OutState)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(RunUpdateStatus);

	const auto StatusResult = AnchorpointCliOperations::GetStatus();
	if (StatusResult.HasError())
	{
		return false;
	}

	const auto CurrentUserResult = AnchorpointCliOperations::GetCurrentUser();
	if (CurrentUserResult.HasError())
	{
		return false;
	}

	auto IsRelevant = [&InputPaths](const FString& FileToCheck)
	{
		for (const FString& Path : InputPaths)
		{
			if (FileToCheck.Compare(Path, ESearchCase::IgnoreCase) == 0)
			{
				return true;
			}

			if (FPaths::IsUnderDirectory(FileToCheck, Path))
			{
				return true;
			}
		}

		return false;
	};

	const FAnchorpointStatus Status = StatusResult.GetValue();

	for (const FString& File : Status.GetAllAffectedFiles())
	{
		if (!IsRelevant(File))
		{
			continue;
		}

		FAnchorpointSourceControlState& NewState = OutState.Emplace_GetRef(File);

		const FString* LockedBy = Status.Locked.Find(File);
		const bool bLockedByMe = LockedBy && *LockedBy == CurrentUserResult.GetValue();

		//TODO: Discuss what the priorities should be here - can I have a file in Add/Modified while else has it locked?
		if (auto StagedState = Status.Staged.Find(File))
		{
			switch (*StagedState)
			{
			case EAnchorpointFileOperation::Added:
				NewState.State = bLockedByMe ? EAnchorpointState::LockedAdded : EAnchorpointState::UnlockedAdded;
				break;
			case EAnchorpointFileOperation::Modified:
				NewState.State = bLockedByMe ? EAnchorpointState::LockedModified : EAnchorpointState::UnlockedModified;
				break;
			case EAnchorpointFileOperation::Deleted:
				NewState.State = bLockedByMe ? EAnchorpointState::LockedDeleted : EAnchorpointState::UnlockedDeleted;
				break;
			default:
				NewState.State = EAnchorpointState::Unknown;
			}
		}
		else if (auto NotStagedState = Status.NotStaged.Find(File))
		{
			switch (*NotStagedState)
			{
			case EAnchorpointFileOperation::Added:
				NewState.State = bLockedByMe ? EAnchorpointState::LockedAdded : EAnchorpointState::UnlockedAdded;
				break;
			case EAnchorpointFileOperation::Modified:
				NewState.State = bLockedByMe ? EAnchorpointState::LockedModified : EAnchorpointState::UnlockedModified;
				break;
			case EAnchorpointFileOperation::Deleted:
				NewState.State = bLockedByMe ? EAnchorpointState::LockedDeleted : EAnchorpointState::UnlockedDeleted;
				break;
			default:
				NewState.State = EAnchorpointState::Unknown;
			}
		}
		else if (Status.Outdated.Contains(File))
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
				NewState.State = EAnchorpointState::LockedBySomeone;
				NewState.OtherUserCheckedOut = *LockedBy;
			}
		}
		else
		{
			NewState.State = EAnchorpointState::UnlockedUnchanged;
		}
	}

	// In case we have files that are not in the status, we add them as UnlockedUnchanged
	for (const FString& Path : InputPaths)
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

	TValueOrError<FString, FString> ConnectResult = AnchorpointCliOperations::Connect();

	if (ConnectResult.HasError())
	{
		InCommand.ErrorMessages.Add(ConnectResult.GetError());
	}

	InCommand.bCommandSuccessful = ConnectResult.HasValue();
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

	TValueOrError<FString, FString> LockResult = AnchorpointCliOperations::LockFiles(InCommand.Files);

	if (LockResult.HasError())
	{
		InCommand.ErrorMessages.Add(LockResult.GetError());
	}

	InCommand.bCommandSuccessful = LockResult.HasValue();
	InCommand.bCommandSuccessful &= RunUpdateStatus(InCommand.Files, States);
	UpdateCachedStates(States);

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCheckOutWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckOutWorker::UpdateStates);

	return UpdateCachedStates(States);
}

FName FAnchorpointRevertWorker::GetName() const
{
	return TEXT("Revert");
}

bool FAnchorpointRevertWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointRevertWorker::Execute);

	TSharedRef<FRevert> RevertOperation = StaticCastSharedRef<FRevert>(InCommand.Operation);

	TValueOrError<FString, FString> UnlockResult = AnchorpointCliOperations::UnlockFiles(InCommand.Files);

	if (UnlockResult.HasError())
	{
		InCommand.ErrorMessages.Add(UnlockResult.GetError());
	}

	InCommand.bCommandSuccessful = UnlockResult.HasValue();

	if (!RevertOperation->IsSoftRevert())
	{
		TValueOrError<FString, FString> DiscardChangesResult = AnchorpointCliOperations::DiscardChanges(InCommand.Files);

		if (DiscardChangesResult.HasError())
		{
			InCommand.ErrorMessages.Add(DiscardChangesResult.GetError());
		}

		InCommand.bCommandSuccessful &= DiscardChangesResult.HasValue();
	}

	InCommand.bCommandSuccessful &= RunUpdateStatus(InCommand.Files, States);

	UpdateCachedStates(States);

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointRevertWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointRevertWorker::UpdateStates);

	return UpdateCachedStates(States);
}

FName FAnchorpointUpdateStatusWorker::GetName() const
{
	return "UpdateStatus";
}

bool FAnchorpointUpdateStatusWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointUpdateStatusWorker::Execute);

	InCommand.bCommandSuccessful = RunUpdateStatus(InCommand.Files, States);
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointUpdateStatusWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointUpdateStatusWorker::UpdateStates);

	return UpdateCachedStates(States);
}

FName FAnchorpointAddWorker::GetName() const
{
	return "Add";
}

bool FAnchorpointAddWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointAddWorker::Execute);

	TValueOrError<FString, FString> LockResult = AnchorpointCliOperations::LockFiles(InCommand.Files);

	if (LockResult.HasError())
	{
		InCommand.ErrorMessages.Add(LockResult.GetError());
	}

	InCommand.bCommandSuccessful = LockResult.HasValue();
	InCommand.bCommandSuccessful &= RunUpdateStatus(InCommand.Files, States);
	UpdateCachedStates(States);

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointAddWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointAddWorker::UpdateStates);

	return UpdateCachedStates(States);
}

FName FAnchorpointCopyWorker::GetName() const
{
	return "Copy";
}

bool FAnchorpointCopyWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCopyWorker::Execute);

	InCommand.bCommandSuccessful &= RunUpdateStatus(InCommand.Files, States);
	UpdateCachedStates(States);

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCopyWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCopyWorker::UpdateStates);

	return UpdateCachedStates(States);
}

FName FAnchorpointDeleteWorker::GetName() const
{
	return "Delete";
}

bool FAnchorpointDeleteWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointDeleteWorker::Execute);

	TValueOrError<FString, FString> LockResult = AnchorpointCliOperations::LockFiles(InCommand.Files);

	if (LockResult.HasError())
	{
		InCommand.ErrorMessages.Add(LockResult.GetError());
	}

	InCommand.bCommandSuccessful = LockResult.HasValue();

	TValueOrError<FString, FString> DeleteResult = AnchorpointCliOperations::DeleteFiles(InCommand.Files);

	if (DeleteResult.HasError())
	{
		InCommand.ErrorMessages.Add(DeleteResult.GetError());
	}

	InCommand.bCommandSuccessful &= DeleteResult.HasValue();

	InCommand.bCommandSuccessful = RunUpdateStatus(InCommand.Files, States);
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointDeleteWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointDeleteWorker::UpdateStates);

	return UpdateCachedStates(States);
}

FName FAnchorpointCheckInWorker::GetName() const
{
	return "CheckIn";
}

bool FAnchorpointCheckInWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckInWorker::Execute);

	TSharedRef<FCheckIn> Operation = StaticCastSharedRef<FCheckIn>(InCommand.Operation);
	const FString Message = Operation->GetDescription().ToString();

	TValueOrError<FString, FString> SubmitResult = AnchorpointCliOperations::SubmitFiles(InCommand.Files, Message);

	if (SubmitResult.HasError())
	{
		InCommand.ErrorMessages.Add(SubmitResult.GetError());
	}

	InCommand.bCommandSuccessful &= SubmitResult.HasValue();

	TValueOrError<FString, FString> UnlockResult = AnchorpointCliOperations::UnlockFiles(InCommand.Files);

	if (UnlockResult.HasError())
	{
		InCommand.ErrorMessages.Add(UnlockResult.GetError());
	}

	InCommand.bCommandSuccessful = UnlockResult.HasValue();

	InCommand.bCommandSuccessful &= RunUpdateStatus(InCommand.Files, States);
	UpdateCachedStates(States);

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCheckInWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckInWorker::UpdateStates);

	return UpdateCachedStates(States);
}