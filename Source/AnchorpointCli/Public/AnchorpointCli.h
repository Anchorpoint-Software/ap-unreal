// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <Modules/ModuleInterface.h>

using FGetInstallFolderDelegate = TDelegate<FString()>;

class ANCHORPOINTCLI_API FAnchorpointCliModule final : public IModuleInterface
{
public:
	static FAnchorpointCliModule& Get();

	FString GetInstallFolder() const;
	FString GetCliPath() const;
	FString GetApplicationPath() const;

	/**
	 * Delegate executed when the `Connect` source control command is executed by Anchorpoint
	 */
	FSimpleMulticastDelegate OnAnchorpointConnected;
};
