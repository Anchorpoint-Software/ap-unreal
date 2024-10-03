// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Widgets/SCompoundWidget.h>

/**
 * Widget used to display the configurable settings inside the Source Control connection dialog. 
 */
class SAnchorpointSourceControlSettingsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAnchorpointSourceControlSettingsWidget)
		{
		}

	SLATE_END_ARGS()

	/*
	 * Constructs this settings widget with the given parameters.
	 */
	void Construct(const FArguments& InArgs);

private:
	//TODO: In the future we want to validate there an actual CLI .exe in the folder before allowing him to save the settings. 
	/**
	 * Callback executed when the user sets a custom install directory for Anchorpoint.
	 */
	void OnInstallDirectoryTextCommited(const FText& Text, ETextCommit::Type Arg);
	/**
	 * Callback executed to determine the currently selected install directory for Anchorpoint.
	 */
	FText GetInstallDirectoryText() const;
};