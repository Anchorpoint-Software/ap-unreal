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
	static FAnchorpointStatus FromString(const FString& InString);
	
	FString CurrentBranch;
	TMap<FString, EAnchorpointFileOperation> Staged;
	TMap<FString, EAnchorpointFileOperation> NotStaged;
	TMap<FString, FString> LockedFiles;
	TArray<FString> OutdatedFiles;
};

namespace AnchorpointCliOperations
{
	TValueOrError<FString, FString> ANCHORPOINTCLI_API Connect();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API GetCurrentUser();
	TValueOrError<FAnchorpointStatus, FString> ANCHORPOINTCLI_API GetStatus();
	TValueOrError<FString, FString> ANCHORPOINTCLI_API LockFiles(TArray<FString>& InFiles);
}
