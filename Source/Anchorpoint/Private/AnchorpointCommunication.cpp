#include "AnchorpointCommunication.h"

#include "Anchorpoint.h"
#include "AnchorpointControlState.h"
#include "JsonObjectConverter.h"

FAnchorpointStatus AnchorpointCommunication::GetStatus()
{
	const FString MockData = TEXT(R"(
	{
		"current_branch": "feature/xyz",
		"staged:": {
			"Content/TopDown/Blueprints/BP_TopDownCharacter.uasset": "A"
		},
		"not_staged": {
			"Content/TopDown/Blueprints/BP_TopDownPlayerController.uasset": "A"
		},
		"locked_files": {
			"Content/TopDown/Maps/TopDownMap/TopDownMap_BuiltData.uasset": "user@anchorpoint.app"
		},
		"outdated_files": ["Content/TopDown/Maps/TopDownMap.umap"]
	}
	)");

	FJsonObjectWrapper JsonObject;
	JsonObject.JsonObjectFromString(MockData);

	FAnchorpointStatus Result;
	Result.current_branch = JsonObject.JsonObject->GetStringField(TEXT("current_branch"));
	for(const TTuple<FString, TSharedPtr<FJsonValue>> StagedFile : JsonObject.JsonObject->GetObjectField(TEXT("staged"))->Values)
	{
		Result.staged.Add(StagedFile.Key, StagedFile.Value->AsString());
	}
	for(const TTuple<FString, TSharedPtr<FJsonValue>> NotStagedFile : JsonObject.JsonObject->GetObjectField(TEXT("not_staged"))->Values)
	{
		Result.not_staged.Add(NotStagedFile.Key, NotStagedFile.Value->AsString());
	}
	for (const TSharedPtr<FJsonValue> OutDatedFile : JsonObject.JsonObject->GetArrayField(TEXT("outdated_files")))
	{
		Result.outdated_files.Add(OutDatedFile->AsString());
	}

	return Result;
}

TMap<FString, EAnchorpointState> AnchorpointCommunication::GetStatusFileStates()
{
	TMap<FString, EAnchorpointState> Result;

	const FString ProjectRoot = FPaths::ProjectDir();

	const FAnchorpointStatus Status = GetStatus();
	for (const auto& StagedFile : Status.staged)
	{
		const FString State = StagedFile.Value;

		if (State == TEXT("A"))
		{
			Result.Emplace(ProjectRoot / StagedFile.Key, EAnchorpointState::Added);
		}
	}
	for (const auto& NotStagedFile : Status.not_staged)
	{
		Result.Emplace(ProjectRoot / NotStagedFile.Key, EAnchorpointState::Modified);
	}
	for (const auto& LockedFile : Status.locked_files)
	{
		Result.Emplace(ProjectRoot / LockedFile.Key, EAnchorpointState::Locked);
	}
	for (const auto& OutdatedFile : Status.outdated_files)
	{
		Result.Emplace(ProjectRoot / OutdatedFile, EAnchorpointState::OutDated);
	}

	return Result;
}

bool AnchorpointCommunication::RunUpdateStatus(const TArray<FString>& InFiles, TArray<FString>& OutErrorMessages, TArray<FAnchorpointControlState>& OutState)
{
	const TMap<FString, EAnchorpointState> FileStates = GetStatusFileStates();
	for (const TTuple<FString, EAnchorpointState> FileState : FileStates)
	{
		FAnchorpointControlState& NewState = OutState.Emplace_GetRef(FileState.Key);
		NewState.State = FileState.Value;
	}

	return true;
}