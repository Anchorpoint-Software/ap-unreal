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
		if(InputPaths.IsEmpty())
		{
			return true;
		}

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

	TValueOrError<FString, FString> CurrentUserResult = AnchorpointCliOperations::GetCurrentUser();

	if (CurrentUserResult.HasError())
	{
		InCommand.ErrorMessages.Add(CurrentUserResult.GetError());
	}

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

	TValueOrError<FString, FString> LockResult = AnchorpointCliOperations::LockFiles(InCommand.Files);

	if (LockResult.HasError())
	{
		InCommand.ErrorMessages.Add(LockResult.GetError());
	}

	InCommand.bCommandSuccessful = LockResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCheckOutWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckOutWorker::UpdateStates);

	return true;
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

	TValueOrError<FString, FString> RevertResult = AnchorpointCliOperations::Revert(InCommand.Files);

	if (RevertResult.HasError())
	{
		InCommand.ErrorMessages.Add(RevertResult.GetError());
	}

	InCommand.bCommandSuccessful = RevertResult.HasValue();
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

	//TODO: do we need to do anything here? we are going to get an update status call at save callback

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

	//TODO: do we need to do anything here? we are going to get an update status call at save callback

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

	//TODO: Since we are running the lock with --git & --keep should we unlock the files automatically?

	TSharedRef<FCheckIn> Operation = StaticCastSharedRef<FCheckIn>(InCommand.Operation);
	const FString Message = Operation->GetDescription().ToString();

	TValueOrError<FString, FString> SubmitResult = AnchorpointCliOperations::SubmitFiles(InCommand.Files, Message);

	if (SubmitResult.HasError())
	{
		InCommand.ErrorMessages.Add(SubmitResult.GetError());
	}

	InCommand.bCommandSuccessful = SubmitResult.HasValue();
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCheckInWorker::UpdateStates() const
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointCheckInWorker::UpdateStates);

	return true;
}