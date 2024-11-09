#pragma once

#include "AnchorpointCliStatus.h"

namespace AnchorpointCliOperations
{
	bool ANCHORPOINTCLI_API IsInstalled();
	void ANCHORPOINTCLI_API ShowInAnchorpoint(const FString& InPath);

	FString ANCHORPOINTCLI_API GetRepositoryRootPath();
	FString ANCHORPOINTCLI_API ConvertFullPathToApInternal(const FString& InFullPath);
	FString ANCHORPOINTCLI_API ConvertApInternalToFull(const FString& InRelativePath);

	TValueOrError<FString, FString> ANCHORPOINTCLI_API GetCurrentUser();

	TValueOrError<FAnchorpointStatus, FString> ANCHORPOINTCLI_API GetStatus(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API LockFiles(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API UnlockFiles(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API Revert(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API DeleteFiles(const TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API SubmitFiles(const TArray<FString> InFiles, const FString& InMessage);
}