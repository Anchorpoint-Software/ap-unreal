#include "AnchorpointCli.h"

FAnchorpointCliModule& FAnchorpointCliModule::Get()
{
	return FModuleManager::LoadModuleChecked<FAnchorpointCliModule>("AnchorpointCli");
}

FString FAnchorpointCliModule::GetInstallFolder() const
{
	if (GetInstallFolderDelegate.IsBound())
	{
		return GetInstallFolderDelegate.Execute();
	}

	return {};
}

FString FAnchorpointCliModule::GetCliPath() const
{
	const FString InstallFolder = GetInstallFolder();

#if PLATFORM_WINDOWS
	return InstallFolder / "ap.exe";
#elif PLATFORM_MAC
	return InstallFolder / "ap";
#endif
}

FString FAnchorpointCliModule::GetApplicationPath() const
{
	const FString InstallFolder = GetInstallFolder();

#if PLATFORM_WINDOWS
	return InstallFolder / "Anchorpoint.exe";
#elif PLATFORM_MAC
	return InstallFolder / "Anchorpoint";
#endif
}

IMPLEMENT_MODULE(FAnchorpointCliModule, AnchorpointCli)