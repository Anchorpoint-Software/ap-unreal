// Copyright (C) 2024-2025 Anchorpoint GmbH. All rights reserved.

#include "AnchorpointSourceControlRevision.h"

#include <HAL/FileManager.h>
#include <Misc/Paths.h>

#include "AnchorpointCliOperations.h"

FAnchorpointSourceControlRevision::FAnchorpointSourceControlRevision(const FAnchorpointHistoryEntry& InHistoryEntry, const FString& InFilename, int32 InRevisionNumber)
{
	Filename = InFilename;
	CommitId = InHistoryEntry.Commit;
	ShortCommitId = CommitId.Left(8);
	CommitIdNumber = FParse::HexNumber(*ShortCommitId);
	Author = InHistoryEntry.Author;
	Date = InHistoryEntry.Date;
	Messsage = InHistoryEntry.Message;
	RevisionNumber = InRevisionNumber;
}

bool FAnchorpointSourceControlRevision::Get(FString& InOutFilename, EConcurrency::Type InConcurrency) const
{
	// If a filename for the temp file wasn't supplied generate a unique-ish one
	// TODO: I don't particularly like this - if we have 2 files with the same name in different directories it will fail.
	if (InOutFilename.Len() == 0)
	{
		IFileManager::Get().MakeDirectory(*FPaths::DiffDir(), true);
		const FString TempFileName = FPaths::DiffDir() / FString::Printf(TEXT("temp-%s-%s"), *CommitId, *FPaths::GetCleanFilename(Filename));
		InOutFilename = FPaths::ConvertRelativePathToFull(TempFileName);
	}

	if (FPaths::FileExists(InOutFilename))
	{
		// File has been previously downloaded so we assume it's still valid
		return true;
	}

	TValueOrError<FString, FString> DownloadResult = AnchorpointCliOperations::DownloadFile(CommitId, Filename, InOutFilename);
	return DownloadResult.HasValue();
}

bool FAnchorpointSourceControlRevision::GetAnnotated(TArray<FAnnotationLine>& OutLines) const
{
	return false;
}

bool FAnchorpointSourceControlRevision::GetAnnotated(FString& InOutFilename) const
{
	return false;
}

const FString& FAnchorpointSourceControlRevision::GetFilename() const
{
	return Filename;
}

int32 FAnchorpointSourceControlRevision::GetRevisionNumber() const
{
	return RevisionNumber;
}

const FString& FAnchorpointSourceControlRevision::GetRevision() const
{
	return ShortCommitId;
}

const FString& FAnchorpointSourceControlRevision::GetDescription() const
{
	return Messsage;
}

const FString& FAnchorpointSourceControlRevision::GetUserName() const
{
	return Author;
}

const FString& FAnchorpointSourceControlRevision::GetClientSpec() const
{
	static FString EmptyString(TEXT(""));
	return EmptyString;
}

const FString& FAnchorpointSourceControlRevision::GetAction() const
{
	return Action;
}

TSharedPtr<ISourceControlRevision> FAnchorpointSourceControlRevision::GetBranchSource() const
{
	return BranchSource;
}

const FDateTime& FAnchorpointSourceControlRevision::GetDate() const
{
	return Date;
}

int32 FAnchorpointSourceControlRevision::GetCheckInIdentifier() const
{
	return CommitIdNumber;
}

int32 FAnchorpointSourceControlRevision::GetFileSize() const
{
	return FileSize;
}

#undef LOCTEXT_NAMESPACE