// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointCliStatus.h"

#include "AnchorpointCliOperations.h"

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

	FAnchorpointStatus Result;
	if (FString CurrentBranch; InJsonObject->TryGetStringField(TEXT("current_branch"), CurrentBranch))
	{
		Result.CurrentBranch = CurrentBranch;
	}

	if (const TSharedPtr<FJsonObject>* Staged; InJsonObject->TryGetObjectField(TEXT("staged"), Staged))
	{
		for (const TTuple<FString, TSharedPtr<FJsonValue>> StagedFile : (*Staged)->Values)
		{
			FString FullFilePath = AnchorpointCliOperations::ConvertApInternalToFull(StagedFile.Key);
			Result.Staged.Add(FullFilePath, LexFromString(StagedFile.Value->AsString()));
		}
	}

	if (const TSharedPtr<FJsonObject>* NotStaged; InJsonObject->TryGetObjectField(TEXT("not_staged"), NotStaged))
	{
		for (const TTuple<FString, TSharedPtr<FJsonValue>> NotStagedFile : (*NotStaged)->Values)
		{
			FString FullFilePath = AnchorpointCliOperations::ConvertApInternalToFull(NotStagedFile.Key);
			Result.NotStaged.Add(FullFilePath, LexFromString(NotStagedFile.Value->AsString()));
		}
	}

	if (const TSharedPtr<FJsonObject>* LockedFiles; InJsonObject->TryGetObjectField(TEXT("locked_files"), LockedFiles))
	{
		for (TTuple<FString, TSharedPtr<FJsonValue>> LockedFile : (*LockedFiles)->Values)
		{
			FString FullFilePath = AnchorpointCliOperations::ConvertApInternalToFull(LockedFile.Key);
			Result.Locked.Add(FullFilePath, LockedFile.Value->AsString());
		}
	}

	if (const TArray<TSharedPtr<FJsonValue>>* OutdatedFiles; InJsonObject->TryGetArrayField(TEXT("outdated_files"), OutdatedFiles))
	{
		for (const TSharedPtr<FJsonValue> OutDatedFile : *OutdatedFiles)
		{
			FString FullFilePath = AnchorpointCliOperations::ConvertApInternalToFull(OutDatedFile->AsString());
			Result.Outdated.Add(FullFilePath);
		}
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

TArray<FAnchorpointHistoryEntry> FAnchorpointHistoryEntry::ArrayFromJsonArray(const TArray<TSharedPtr<FJsonValue>>& HistoryEntries)
{
	TRACE_CPUPROFILER_EVENT_SCOPE(FAnchorpointHistoryEntry::ArrayFromJsonArray);

	TArray<FAnchorpointHistoryEntry> Result;

	for (const TSharedPtr<FJsonValue>& HistoryEntry : HistoryEntries)
	{
		TSharedPtr<FJsonObject> HistoryObject = HistoryEntry->AsObject();

		FAnchorpointHistoryEntry Entry;
		Entry.Commit = HistoryObject->GetStringField(TEXT("commit"));
		Entry.Author = HistoryObject->GetStringField(TEXT("author"));
		Entry.Message = HistoryObject->GetStringField(TEXT("message"));
		Entry.Date = FDateTime::FromUnixTimestamp(HistoryObject->GetNumberField(TEXT("date")));

		Result.Add(Entry);
	}

	return Result;
}