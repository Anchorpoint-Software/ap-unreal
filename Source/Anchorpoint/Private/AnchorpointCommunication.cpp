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

	FAnchorpointStatus Result;
	const bool bSuccess = FJsonObjectConverter::JsonObjectStringToUStruct(MockData, &Result);
	ensure(bSuccess);

	return Result;
}

TMap<FString, EAnchorpointState> AnchorpointCommunication::GetStatusFileStates()
{
	TMap<FString, EAnchorpointState> Result;

	const FString ProjectRoot = FPaths::ProjectDir();

	const FAnchorpointStatus Status = GetStatus();
	for(const auto& StagedFile : Status.staged)
	{
		Result.Emplace(ProjectRoot / StagedFile.Key, EAnchorpointState::Staged);
	}
	for(const auto& NotStagedFile : Status.not_staged)
	{
		Result.Emplace(ProjectRoot / NotStagedFile.Key, EAnchorpointState::Modified);
	}
	for(const auto& LockedFile : Status.locked_files)
	{
		Result.Emplace(ProjectRoot / LockedFile.Key, EAnchorpointState::Locked);
	}
	for(const auto& OutdatedFile : Status.outdated_files)
	{
		Result.Emplace(ProjectRoot / OutdatedFile, EAnchorpointState::OutDated);
	}

	return Result;
}

bool AnchorpointCommunication::RunUpdateStatus(const TArray<FString>& InFiles, TArray<FString>& OutErrorMessages, TArray<FAnchorpointControlState>& OutState)
{
	const TMap<FString, EAnchorpointState> FileStates = GetStatusFileStates();
	for(const TTuple<FString, EAnchorpointState> FileState : FileStates)
	{
		FAnchorpointControlState& NewState = OutState.Emplace_GetRef(FileState.Key);
		NewState.State = FileState.Value;
	}

	return true;
}