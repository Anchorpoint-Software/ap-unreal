// Some copyright should be here...

#include "AnchorpointSouceControlOperations.h"

#include <SourceControlOperations.h>

#include "Anchorpoint.h"
#include "AnchorpointCliOperations.h"
#include "AnchorpointControlCommand.h"
#include "AnchorpointControlState.h"
#include "AnchorpointSourceControlProvider.h"

bool UpdateCachedStates(const TArray<FAnchorpointControlState>& InStates)
{
	FAnchorpointSourceControlProvider& Provider = FAnchorpointModule::Get().GetProvider();

	for (const FAnchorpointControlState& InState : InStates)
	{
		TSharedRef<FAnchorpointControlState> State = Provider.GetStateInternal(InState.LocalFilename);
		*State = InState;
		State->TimeStamp = FDateTime::Now();
	}

	return !InStates.IsEmpty();
}

bool RunUpdateStatus(const TArray<FString>& InFiles, TArray<FAnchorpointControlState>& OutState)
{
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

	const FAnchorpointStatus Status = StatusResult.GetValue();

	for (const FString& File : InFiles)
	{
		FAnchorpointControlState& NewState = OutState.Emplace_GetRef(File);

		const FString* LockedBy = Status.LockedFiles.Find(File);
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
		else if (Status.OutdatedFiles.Contains(File))
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

	return true;
}

FName FAnchorpointConnectWorker::GetName() const
{
	return "Connect";
}

bool FAnchorpointConnectWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
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
	return true;
}

FName FAnchorpointCheckOutWorker::GetName() const
{
	return "CheckOut";
}

bool FAnchorpointCheckOutWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	TValueOrError<FString, FString> CheckoutResult = AnchorpointCliOperations::LockFiles(InCommand.Files);

	if (CheckoutResult.HasError())
	{
		InCommand.ErrorMessages.Add(CheckoutResult.GetError());
	}

	InCommand.bCommandSuccessful = CheckoutResult.HasValue();
	InCommand.bCommandSuccessful &= RunUpdateStatus(InCommand.Files, States);
	UpdateCachedStates(States);

	return InCommand.bCommandSuccessful;
}

bool FAnchorpointCheckOutWorker::UpdateStates() const
{
	return UpdateCachedStates(States);
}

FName FAnchorpointRevertWorker::GetName() const
{
	return TEXT("Revert");
}

bool FAnchorpointRevertWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
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
	return UpdateCachedStates(States);
}

FName FAnchorpointUpdateStatusWorker::GetName() const
{
	return "UpdateStatus";
}

bool FAnchorpointUpdateStatusWorker::Execute(FAnchorpointSourceControlCommand& InCommand)
{
	InCommand.bCommandSuccessful = RunUpdateStatus(InCommand.Files, States);
	return InCommand.bCommandSuccessful;
}

bool FAnchorpointUpdateStatusWorker::UpdateStates() const
{
	return UpdateCachedStates(States);
}