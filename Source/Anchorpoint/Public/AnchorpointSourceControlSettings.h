// Fill out your copyright notice in the Description page of Project Settings.

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
