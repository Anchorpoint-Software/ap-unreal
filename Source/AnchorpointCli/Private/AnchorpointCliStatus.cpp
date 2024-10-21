#include "AnchorpointCliStatus.h"

EAnchorpointFileOperation LexFromString(const FString& InString)
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
	if (InString == TEXT("R"))
	{
		return EAnchorpointFileOperation::Renamed;
	}
	if (InString == TEXT("C"))
	{
		return EAnchorpointFileOperation::Conflicted;
	}

	return EAnchorpointFileOperation::Invalid;
}

FAnchorpointStatus FAnchorpointStatus::FromJson(const TSharedRef<FJsonObject>& InJsonObject)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointStatus::FromString);

	const FString ProjectPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

	FAnchorpointStatus Result;
	Result.CurrentBranch = InJsonObject->GetStringField(TEXT("current_branch"));
	for (const TTuple<FString, TSharedPtr<FJsonValue>> StagedFile : InJsonObject->GetObjectField(TEXT("staged"))->Values)
	{
		FString FullFilePath = ProjectPath / StagedFile.Key;
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.Staged.Add(FullFilePath, LexFromString(StagedFile.Value->AsString()));
	}
	for (const TTuple<FString, TSharedPtr<FJsonValue>> NotStagedFile : InJsonObject->GetObjectField(TEXT("not_staged"))->Values)
	{
		FString FullFilePath = ProjectPath / NotStagedFile.Key;
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.NotStaged.Add(FullFilePath, LexFromString(NotStagedFile.Value->AsString()));
	}
	for (TTuple<FString, TSharedPtr<FJsonValue>> LockedFile : InJsonObject->GetObjectField(TEXT("locked_files"))->Values)
	{
		FString FullFilePath = ProjectPath / LockedFile.Key;
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.Locked.Add(FullFilePath, LockedFile.Value->AsString());
	}
	for (const TSharedPtr<FJsonValue> OutDatedFile : InJsonObject->GetArrayField(TEXT("outdated_files")))
	{
		FString FullFilePath = ProjectPath / OutDatedFile->AsString();
		FPaths::NormalizeDirectoryName(FullFilePath);
		Result.Outdated.Add(FullFilePath);
	}

	return Result;
}

TArray<FString> FAnchorpointStatus::GetAllAffectedFiles() const
{
	TArray<FString> Result;

	TArray<FString> StagedFiles;
	Staged.GetKeys(StagedFiles);
	Result.Append(StagedFiles);

	TArray<FString> NotStagedFiles;
	NotStaged.GetKeys(NotStagedFiles);
	Result.Append(NotStagedFiles);

	TArray<FString> LockedFiles;
	Locked.GetKeys(LockedFiles);
	Result.Append(LockedFiles);

	Result.Append(Outdated);

	return Result;
}