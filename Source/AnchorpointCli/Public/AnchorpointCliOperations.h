#pragma once

#include "AnchorpointCliStatus.h"

namespace AnchorpointCliOperations
{
	bool ANCHORPOINTCLI_API IsInstalled();
	void ANCHORPOINTCLI_API ShowInAnchorpoint(const FString& InPath);

	TValueOrError<FAnchorpointStatus, FString> ANCHORPOINTCLI_API GetStatus();

	TValueOrError<FString, FString> ANCHORPOINTCLI_API GetCurrentUser();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API LockFiles(TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API UnlockFiles(TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API Revert(TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API DeleteFiles(TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API SubmitFiles(TArray<FString> InFiles, const FString& InMessage);
}