// Some copyright should be here...

#pragma once

enum class EAnchorpointFileOperation
{
	Invalid,
	Added,
	Modified,
	Deleted,
	Renamed,
	Conflicted,
};

EAnchorpointFileOperation LexFromString(const FString& InString);

struct FAnchorpointStatus
{
	static FAnchorpointStatus FromJson(const TSharedRef<FJsonObject>& InJsonObject);

	FString CurrentBranch;
	TMap<FString, EAnchorpointFileOperation> Staged;
	TMap<FString, EAnchorpointFileOperation> NotStaged;
	TMap<FString, FString> Locked;
	TArray<FString> Outdated;

	ANCHORPOINTCLI_API TArray<FString> GetAllAffectedFiles() const;
};

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
	TValueOrError<FString, FString> ANCHORPOINTCLI_API Connect();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API GetCurrentUser();
	TValueOrError<FAnchorpointStatus, FString> ANCHORPOINTCLI_API GetStatus();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API LockFiles(TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API UnlockFiles(TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API DiscardChanges(TArray<FString>& InFiles);
	TValueOrError<FString, FString> ANCHORPOINTCLI_API DeleteFiles(TArray<FString>& InFiles);

	FString GetCliPath();
	FCliResult RunApCommand(const FString& InCommand, bool bRequestJsonOutput = true);
	FCliResult RunGitCommand(const FString& InCommand);

}