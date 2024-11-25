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
	/**
	 * Callback executed to determine if Anchorpoint has a valid installation path 
	 */
	bool IsSoftwareInstalled() const;
};