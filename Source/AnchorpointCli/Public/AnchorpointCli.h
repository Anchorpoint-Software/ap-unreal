// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <Modules/ModuleInterface.h>

using FGetInstallFolderDelegate = TDelegate<FString()>;

class ANCHORPOINTCLI_API FAnchorpointCliModule final : public IModuleInterface
{
public:
	static FAnchorpointCliModule& Get();
	static FAnchorpointCliModule* GetPtr();

	FString GetInstallFolder() const;
	FString GetCliPath() const;
	FString GetApplicationPath() const;

	bool IsCurrentProvider() const;
};
