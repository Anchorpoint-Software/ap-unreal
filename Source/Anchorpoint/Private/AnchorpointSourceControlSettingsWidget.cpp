// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointSourceControlSettingsWidget.h"

#include "Anchorpoint.h"
#include "AnchorpointCli.h"
#include "AnchorpointCliUtils.h"
#include "AnchorpointSourceControlSettings.h"

void SAnchorpointSourceControlSettingsWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(2.0f)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Anchorpoint", "InstallDirectoryLabel", "Path"))
				.ToolTipText(NSLOCTEXT("Anchorpoint", "InstallDirectoryLabel_Tooltip", "Path on disk to the InstallDirectory"))
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(2.0f)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(2.0f)
			[
				SNew(STextBlock)
				.Text(this, &SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText)
				.ToolTipText(this, &SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText)
			]
		]
	];
}

FText SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText() const
{
	const FString InstallPath = FAnchorpointCliModule::Get().GetInstallFolder();
	return FText::FromString(InstallPath);
}
