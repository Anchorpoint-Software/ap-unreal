#pragma once

#include "AnchorpointCliStatus.h"

struct FCliResult
{
	FString Output;
	TOptional<FString> Error;

	bool DidSucceed() const;
	TSharedPtr<FJsonObject> OutputAsJsonObject() const;
	TArray<TSharedPtr<FJsonValue>> OutputAsJsonArray() const;
};

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

	FCliResult RunApCommand(const FString& InCommand, bool bRequestJsonOutput = true);
	FCliResult RunGitCommand(const FString& InCommand);
}