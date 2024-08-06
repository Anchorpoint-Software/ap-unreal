#pragma once
#include "AnchorpointControlState.h"

#include "AnchorpointCommunication.generated.h"

USTRUCT()
struct FAnchorpointStatus
{
	GENERATED_BODY()

	UPROPERTY()
	FString current_branch;

	UPROPERTY()
	TMap<FString, FString> staged;

	UPROPERTY()
	TMap<FString, FString> not_staged;

	UPROPERTY()
	TMap<FString, FString> locked_files;

	UPROPERTY()
	TArray<FString> outdated_files;
};

namespace AnchorpointCommunication
{
	FAnchorpointStatus GetStatus();

	TMap<FString, EAnchorpointState> GetStatusFileStates(); 

	bool RunUpdateStatus(const TArray<FString>& InFiles, TArray<FString>& OutErrorMessages, TArray<FAnchorpointControlState>& OutState);
}

