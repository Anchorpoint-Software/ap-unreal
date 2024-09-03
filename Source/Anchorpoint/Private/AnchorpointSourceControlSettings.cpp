// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointSourceControlSettings.h"

#include "SourceControlHelpers.h"

const FString SettingsSection = TEXT("SourceControl.Anchorpoint");

const FString& FAnchorpointSourceControlSettings::GetInstallDirectory() const
{
	FScopeLock ScopeLock(&CriticalSection);
	return InstallDirectory;
}

void FAnchorpointSourceControlSettings::SetInstallDirectory(const FString& InInstallDirectory)
{
	FScopeLock ScopeLock(&CriticalSection);
	InstallDirectory = InInstallDirectory;
}

void FAnchorpointSourceControlSettings::SaveSettings()
{	FScopeLock ScopeLock(&CriticalSection);
	const FString& IniFile = SourceControlHelpers::GetSettingsIni();
	GConfig->SetString(*SettingsSection, TEXT("InstallDirectory"), *InstallDirectory, IniFile);
}

void FAnchorpointSourceControlSettings::LoadSettings()
{
	FScopeLock ScopeLock(&CriticalSection);
	const FString& IniFile = SourceControlHelpers::GetSettingsIni();
	
	const bool bInstallDirectorySet = GConfig->GetString(*SettingsSection, TEXT("InstallDirectory"), InstallDirectory, IniFile);
	// In case some new variables appears or are unset, let's find some good defaults 
	if(!bInstallDirectorySet)
	{
#if PLATFORM_MAC
		InstallDirectory = TEXT("/Applications/Anchorpoint.app/Contents/Frameworks");
#elif PLATFORM_WINDOWS
		//TODO: In the future we might want to use environment variables, but for now we just scan the user's computer to find an installation
		InstallDirectory = TEXT("C:/Users/oprea/AppData/Local/Anchorpoint/app-1.22.0");
#endif
	}
}