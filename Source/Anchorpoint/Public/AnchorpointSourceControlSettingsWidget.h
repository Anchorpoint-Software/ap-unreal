// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class ANCHORPOINT_API SAnchorpointSourceControlSettingsWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SAnchorpointSourceControlSettingsWidget)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

private:
	FText GetInstallDirectoryText() const;
	void OnInstallDirectoryTextCommited(const FText& Text, ETextCommit::Type Arg);
};