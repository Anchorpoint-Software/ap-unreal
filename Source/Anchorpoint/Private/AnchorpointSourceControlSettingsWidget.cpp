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
		+SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(FMargin(0.0f, 0.0f, 16.0f, 0.0f))
		[
			SNew(SVerticalBox)

			+SVerticalBox::Slot()
			.Padding(FMargin(0.0f, 0.0f, 0.0f, 10.0f))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Anchorpoint", "InstallDirectoryLabel", "Path"))
				.ToolTipText(NSLOCTEXT("Anchorpoint", "InstallDirectoryLabel_Tooltip", "Path on disk to the InstallDirectory"))
			]
			+SVerticalBox::Slot()
			.Padding(FMargin(0.0f, 0.0f, 0.0f, 10.0f))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.Lock"))
			]
		]
		+SHorizontalBox::Slot()
		.FillWidth(4.0f)
		[
			SNew(SVerticalBox)

			+SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(FMargin(0.0f, 0.0f, 0.0f, 10.0f))
			[
				SNew(STextBlock)
				.Text(this, &SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText)
				.ToolTipText(this, &SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText)
			]
			+SVerticalBox::Slot()
			.Padding(FMargin(0.0f, 0.0f, 0.0f, 10.0f))
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Anchorpoint", "UnrealFileLockingWarning", "File locking will be controlled by the Unreal Editor"))
			]
		]
	];
}

FText SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText() const
{
	const FString InstallPath = FAnchorpointCliModule::Get().GetInstallFolder();
	return FText::FromString(InstallPath);
}
