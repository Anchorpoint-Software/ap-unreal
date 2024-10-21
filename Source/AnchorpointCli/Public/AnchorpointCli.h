#pragma once

#include <Modules/ModuleInterface.h>

using FGetInstallFolderDelegate = TDelegate<FString()>;

class ANCHORPOINTCLI_API FAnchorpointCliModule final : public IModuleInterface
{
public:
	static FAnchorpointCliModule& Get();

	FGetInstallFolderDelegate GetInstallFolderDelegate;

	FString GetInstallFolder() const;
	FString GetCliPath() const;
	FString GetApplicationPath() const;
};
