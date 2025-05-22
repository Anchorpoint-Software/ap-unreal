// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include "AnchorpointCliStatus.h"

class FAnchorpointCliProcess;

namespace AnchorpointCliOperations
{
	ANCHORPOINTCLI_API bool IsInstalled();
	ANCHORPOINTCLI_API void ShowInAnchorpoint(const FString& InPath);

	ANCHORPOINTCLI_API FString GetRepositoryRootPath();
	ANCHORPOINTCLI_API bool IsUnderRepositoryPath(const FString& InPath);
	ANCHORPOINTCLI_API FString ConvertFullPathToApInternal(const FString& InFullPath);
	ANCHORPOINTCLI_API FString ConvertApInternalToFull(const FString& InRelativePath);
	ANCHORPOINTCLI_API FString ConvertFullPathToProjectRelative(const FString& InPath);

	ANCHORPOINTCLI_API TValueOrError<FString, FString> GetCurrentUser();

	ANCHORPOINTCLI_API TValueOrError<FAnchorpointStatus, FString> GetStatus(const TArray<FString>& InFiles, bool bForced = false);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> DisableAutoLock();
	ANCHORPOINTCLI_API TValueOrError<FString, FString> LockFiles(const TArray<FString>& InFiles);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> UnlockFiles(const TArray<FString>& InFiles);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> Revert(const TArray<FString>& InFiles);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> DeleteFiles(const TArray<FString>& InFiles);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> SubmitFiles(const TArray<FString> InFiles, const FString& InMessage);

	ANCHORPOINTCLI_API TValueOrError<FAnchorpointConflictStatus, FString> GetConflictStatus(const FString& InFile);
	ANCHORPOINTCLI_API TValueOrError<FAnchorpointHistory, FString> GetHistoryInfo(const FString& InFile);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> DownloadFileViaCommitId(const FString& InCommitId, const FString& InFile, const FString& Destination);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> DownloadFileViaOID(const FString& InOID, const FString& InFile, const FString& Destination);
	ANCHORPOINTCLI_API TValueOrError<FString, FString> MarkConflictSolved(const TArray<FString>& InFiles);

	ANCHORPOINTCLI_API void MonitorProcessWithNotification(const TSharedRef<FAnchorpointCliProcess>& InProcess, const FText& ProgressText, const FText& FinishText);
}