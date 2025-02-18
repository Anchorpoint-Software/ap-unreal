// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <Modules/ModuleManager.h>

#include "AnchorpointSourceControlProvider.h"
#include "AnchorpointSourceControlSettings.h"

class FAnchorpointModule : public IModuleInterface
{
public:
	static FAnchorpointModule& Get();

	FAnchorpointSourceControlProvider& GetProvider();
	FAnchorpointSourceControlSettings& GetSettings();

private:
	// ~Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	// ~End IModuleInterface interface

	void ExtendFileContextMenu(UToolMenu* InMenu);
	void RegisterMergeWithAnchorpoint(FToolMenuSection& InSection, TSoftClassPtr<> AssetSoftClass);

	FAnchorpointSourceControlProvider AnchorpointSourceControlProvider;
	FAnchorpointSourceControlSettings AnchorpointSourceControlSettings;
};

