// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointSourceControlSettings.h"

#include "SourceControlHelpers.h"

const FString SettingsSection = TEXT("SubversionSourceControl.Anchorpoint");

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
	GConfig->GetString(*SettingsSection, TEXT("InstallDirectory"), InstallDirectory, IniFile);

	if(InstallDirectory.IsEmpty())
	{
		InstallDirectory = TEXT("C:/Users/oprea/AppData/Local/Anchorpoint/app-1.22.0"); 
	}
}