#include "AnchorpointCommunication.h"

#include "AnchorpointCliOperations.h"
#include "AnchorpointControlState.h"
#include "JsonObjectConverter.h"

bool AnchorpointCommunication::RunUpdateStatus(const TArray<FString>& InFiles, TArray<FString>& OutErrorMessages, TArray<FAnchorpointControlState>& OutState)
{
	const FAnchorpointStatus Status = AnchorpointCliOperations::GetStatus().GetValue();

	for (const FString& File : InFiles)
	{
		FAnchorpointControlState& NewState = OutState.Emplace_GetRef(File);

		//TODO: Discuss what the priorities should be here - can I have a file in Add/Modified while else has it locked?
		if (Status.OutdatedFiles.Contains(File))
		{
			NewState.State = EAnchorpointState::OutDated;
		}
		else if (auto LockedBy = Status.LockedFiles.Find(File))
		{
			NewState.State = EAnchorpointState::Locked;
			NewState.OtherUserCheckedOut = *LockedBy;
		}
		else if (auto StagedState = Status.Staged.Find(File))
		{
			switch (*StagedState)
			{
			case EAnchorpointFileOperation::Added:
				NewState.State = EAnchorpointState::Added;
				break;
			case EAnchorpointFileOperation::Modified:
				NewState.State = EAnchorpointState::Modified;
				break;
			case EAnchorpointFileOperation::Deleted:
				NewState.State = EAnchorpointState::Deleted;
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
				NewState.State = EAnchorpointState::Added;
				break;
			case EAnchorpointFileOperation::Modified:
				NewState.State = EAnchorpointState::Modified;
				break;
			case EAnchorpointFileOperation::Deleted:
				NewState.State = EAnchorpointState::Deleted;
				break;
			default:
				NewState.State = EAnchorpointState::Unknown;
			}
		}
		else if(Status.OutdatedFiles.Contains(File))
		{
			NewState.State = EAnchorpointState::OutDated;
		}
		else
		{
			NewState.State = EAnchorpointState::Unchanged;
		}
	}

	return true;
}

