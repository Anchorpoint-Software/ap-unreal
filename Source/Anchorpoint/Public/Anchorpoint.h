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

	FString GetCliPath();

	void ExtendFileContextMenu(UToolMenu* InMenu);

	FAnchorpointSourceControlProvider AnchorpointSourceControlProvider;
	FAnchorpointSourceControlSettings AnchorpointSourceControlSettings;
};
