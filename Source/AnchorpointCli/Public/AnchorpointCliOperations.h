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
	TMap<FString, FString> LockedFiles;
	TArray<FString> OutdatedFiles;
};

struct FCliOutput
{
	FString Output;
	TOptional<FString> Error;

	bool DidSucceed() const;
	TSharedPtr<FJsonObject> OutputAsJsonObject() const;
	TArray<TSharedPtr<FJsonValue>> OutputAsJsonArray() const;
};

namespace AnchorpointCliOperations
{
	TValueOrError<FString, FString> ANCHORPOINTCLI_API Connect();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API GetCurrentUser();
	TValueOrError<FAnchorpointStatus, FString> ANCHORPOINTCLI_API GetStatus();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API LockFiles(TArray<FString>& InFiles);

	FCliOutput RunApCli(const FString& InCommand);

}