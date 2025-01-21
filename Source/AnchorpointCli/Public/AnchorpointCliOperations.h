#pragma once

#include "AnchorpointCliStatus.h"

namespace AnchorpointCliOperations
{
	bool ANCHORPOINTCLI_API IsInstalled();
	void ANCHORPOINTCLI_API ShowInAnchorpoint(const FString& InPath);

	FString ANCHORPOINTCLI_API GetRepositoryRootPath();
	bool ANCHORPOINTCLI_API IsUnderRepositoryPath(const FString& InPath);
	FString ANCHORPOINTCLI_API ConvertFullPathToApInternal(const FString& InFullPath);
	FString ANCHORPOINTCLI_API ConvertApInternalToFull(const FString& InRelativePath);

	TValueOrError<FString, FString> ANCHORPOINTCLI_API GetCurrentUser();

	TValueOrError<FAnchorpointStatus, FString> ANCHORPOINTCLI_API GetStatus(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API DisableAutoLock();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API LockFiles(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API UnlockFiles(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API Revert(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API DeleteFiles(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API SubmitFiles(const TArray<FString> InFiles, const FString& InMessage);

	TValueOrError<FAnchorpointHistory, FString> ANCHORPOINTCLI_API GetHistoryInfo(const FString& InFile);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API DownloadFile(const FString& InCommitId, const FString& InFile, const FString& Destination);
}