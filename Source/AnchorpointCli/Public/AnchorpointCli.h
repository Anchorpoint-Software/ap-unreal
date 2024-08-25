// Some copyright should be here...

#pragma once

#include <Modules/ModuleInterface.h>

class ANCHORPOINTCLI_API FAnchorpointCliModule final : public IModuleInterface
{
public:
	static FAnchorpointCliModule& Get();

	TDelegate<FString()> GetCliPathDelegate;
	FString GetCliPath() const;
};
