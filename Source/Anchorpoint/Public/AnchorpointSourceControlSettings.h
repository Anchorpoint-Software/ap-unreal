// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

/**
 * 
 */
class FAnchorpointSourceControlSettings
{
public:
	void SaveSettings();
	void LoadSettings();

private:
	/** A critical section for settings access */
	mutable FCriticalSection CriticalSection;
};
