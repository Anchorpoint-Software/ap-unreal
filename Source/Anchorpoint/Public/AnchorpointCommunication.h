#pragma once
#include "AnchorpointControlState.h"

enum class EAnchorpointFileOperation
{
	Invalid,
	Added,
	Modified,
	Deleted,
};

EAnchorpointFileOperation FromString(const FString& InString);

struct FAnchorpointStatus
{
	FString CurrentBranch;
	TMap<FString, EAnchorpointFileOperation> Staged;
	TMap<FString, EAnchorpointFileOperation> NotStaged;
	TMap<FString, FString> LockedFiles;
	TArray<FString> OutdatedFiles;
};

namespace AnchorpointCommunication
{
	FAnchorpointStatus GetStatus();

	bool RunUpdateStatus(const TArray<FString>& InFiles, TArray<FString>& OutErrorMessages, TArray<FAnchorpointControlState>& OutState);
}