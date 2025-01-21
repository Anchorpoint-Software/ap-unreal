// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

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