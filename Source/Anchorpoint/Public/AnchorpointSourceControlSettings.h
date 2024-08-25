// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AnchorpointSourceControlProvider.h"

/**
 * 
 */
class FAnchorpointSourceControlSettings
{
public:
	const FString& GetInstallDirectory() const;
	void SetInstallDirectory(const FString& InInstallDirectory);
	
	void SaveSettings();
	void LoadSettings();

private:
	/** A critical section for settings access */
	mutable FCriticalSection CriticalSection;
	
	FString InstallDirectory;
};
