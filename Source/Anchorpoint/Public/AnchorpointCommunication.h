#pragma once
#include "AnchorpointControlState.h"

namespace AnchorpointCommunication
{
	bool RunUpdateStatus(const TArray<FString>& InFiles, TArray<FString>& OutErrorMessages, TArray<FAnchorpointControlState>& OutState);
}