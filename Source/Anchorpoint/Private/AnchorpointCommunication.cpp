#include "AnchorpointCommunication.h"

#include "AnchorpointControlState.h"
#include "JsonObjectConverter.h"

FAnchorpointStatus AnchorpointCommunication::GetStatus()
{
	const FString MockData = TEXT(R"(
	{
		"current_branch": "feature/xyz",
		"staged": {
			"Content/TopDown/Blueprints/BP_TopDownCharacter.uasset": "A"
		},
		"not_staged": {
			"Content/TopDown/Blueprints/BP_TopDownPlayerController.uasset": "A",
			"Content/TopDown/Input/Actions/IA_SetDestination_Click.uasset": "M",
			"Content/TopDown/Input/Actions/IA_Test_Click.uasset": "D"
		},
		"locked_files": {
			"Content/TopDown/Maps/TopDownMap/TopDownMap_BuiltData.uasset": "user@anchorpoint.app",
			"Content/TopDown/Input/Actions/IA_SetDestination_Touch.uasset": "user@anchorpoint.app"
		},
		"outdated_files": ["Content/TopDown/Maps/TopDownMap.umap"]
	}
	)");

	const FString ProjectRoot = FPaths::ProjectDir();

	FJsonObjectWrapper JsonObject;
	JsonObject.JsonObjectFromString(MockData);

	FAnchorpointStatus Result;
	Result.CurrentBranch = JsonObject.JsonObject->GetStringField(TEXT("current_branch"));
	for (const TTuple<FString, TSharedPtr<FJsonValue>> StagedFile : JsonObject.JsonObject->GetObjectField(TEXT("staged"))->Values)
	{
		Result.Staged.Add(ProjectRoot / StagedFile.Key, FromString(StagedFile.Value->AsString()));
	}
	for (const TTuple<FString, TSharedPtr<FJsonValue>> NotStagedFile : JsonObject.JsonObject->GetObjectField(TEXT("not_staged"))->Values)
	{
		Result.NotStaged.Add(ProjectRoot / NotStagedFile.Key, FromString(NotStagedFile.Value->AsString()));
	}
	for (TTuple<FString, TSharedPtr<FJsonValue>> LockedFile : JsonObject.JsonObject->GetObjectField(TEXT("locked_files"))->Values)
	{
		Result.LockedFiles.Add(ProjectRoot / LockedFile.Key, LockedFile.Value->AsString());
	}
	for (const TSharedPtr<FJsonValue> OutDatedFile : JsonObject.JsonObject->GetArrayField(TEXT("outdated_files")))
	{
		Result.OutdatedFiles.Add(ProjectRoot / OutDatedFile->AsString());
	}

	return Result;
}

bool AnchorpointCommunication::RunUpdateStatus(const TArray<FString>& InFiles, TArray<FString>& OutErrorMessages, TArray<FAnchorpointControlState>& OutState)
{
	const FAnchorpointStatus Status = GetStatus();

	for (const auto& File : InFiles)
	{
		FAnchorpointControlState& NewState = OutState.Emplace_GetRef(File);

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
			NewState.State = EAnchorpointState::Unknown;
		}
	}

	return true;
}

EAnchorpointFileOperation FromString(const FString& InString)
{
	if (InString == TEXT("A"))
	{
		return EAnchorpointFileOperation::Added;
	}
	if (InString == TEXT("M"))
	{
		return EAnchorpointFileOperation::Modified;
	}
	if (InString == TEXT("D"))
	{
		return EAnchorpointFileOperation::Deleted;
	}

	return EAnchorpointFileOperation::Invalid;
}