// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <Widgets/SCompoundWidget.h>

enum class EAnchorpointInstallationState
{
	NotInstalledOnDesktop,
	NotInstalledInProject,
	Ready
};

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
	 * Determines the status of the Anchorpoint setup for this project & machine
	 */
	EAnchorpointInstallationState GetInstallationState() const;
};