// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointSourceControlSettings.h"

#include <SourceControlHelpers.h>

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
{
	FScopeLock ScopeLock(&CriticalSection);
	const FString& IniFile = SourceControlHelpers::GetSettingsIni();
	GConfig->SetString(*SettingsSection, TEXT("InstallDirectory"), *InstallDirectory, IniFile);
}

bool CompareVersions(const FString& Version1, const FString& Version2)
{
	FString AppPrefix = TEXT("app-");

	TArray<FString> Version1Parts;
	TArray<FString> Version2Parts;

	Version1.RightChop(AppPrefix.Len()).ParseIntoArray(Version1Parts, TEXT("."));
	Version2.RightChop(AppPrefix.Len()).ParseIntoArray(Version2Parts, TEXT("."));

	for (int i = 0; i < FMath::Max(Version1Parts.Num(), Version2Parts.Num()); i++)
	{
		int32 Part1 = (i < Version1Parts.Num()) ? FCString::Atoi(*Version1Parts[i]) : 0;
		int32 Part2 = (i < Version2Parts.Num()) ? FCString::Atoi(*Version2Parts[i]) : 0;

		if (Part1 != Part2)
		{
			return Part1 < Part2;
		}
	}

	return 0;
}

void FAnchorpointSourceControlSettings::LoadSettings()
{
	FScopeLock ScopeLock(&CriticalSection);
	const FString& IniFile = SourceControlHelpers::GetSettingsIni();

	const bool bInstallDirectorySet = GConfig->GetString(*SettingsSection, TEXT("InstallDirectory"), InstallDirectory, IniFile);

	// In case some values are unset, let's find some good defaults 
	if (!bInstallDirectorySet)
	{
#if PLATFORM_MAC
		InstallDirectory = TEXT("/Applications/Anchorpoint.app/Contents/Frameworks");
#elif PLATFORM_WINDOWS
		//TODO: In the future we might want to use a specific use environment, but for now we just scan the user's default installation directory
		FString AppDataLocalPath = FPlatformMisc::GetEnvironmentVariable(TEXT("LOCALAPPDATA"));
		FString AnchorpointVersionsPath = AppDataLocalPath / TEXT("Anchorpoint");

		TArray<FString> AnchorpointVersions;
		IFileManager::Get().FindFiles(AnchorpointVersions, *(AnchorpointVersionsPath / TEXT("*")), false, true);

		Algo::Sort(AnchorpointVersions, CompareVersions);

		FString ChosenVersion = AnchorpointVersionsPath / AnchorpointVersions.Last();
		FPaths::NormalizeDirectoryName(ChosenVersion);

		InstallDirectory = ChosenVersion;
#endif
	}
}