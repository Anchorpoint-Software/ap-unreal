// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointSourceControlSettings.h"

#include <SourceControlHelpers.h>

const FString SettingsSection = TEXT("SourceControl.Anchorpoint");

void FAnchorpointSourceControlSettings::SaveSettings()
{
	FScopeLock ScopeLock(&CriticalSection);
	const FString& IniFile = SourceControlHelpers::GetSettingsIni();
}

void FAnchorpointSourceControlSettings::LoadSettings()
{
	FScopeLock ScopeLock(&CriticalSection);
	const FString& IniFile = SourceControlHelpers::GetSettingsIni();
}