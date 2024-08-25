// Fill out your copyright notice in the Description page of Project Settings.

#include "AnchorpointSourceControlSettingsWidget.h"

#include "Anchorpoint.h"
#include "AnchorpointSourceControlSettings.h"

#define LOCTEXT_NAMESPACE "Anchorpoint"

void SAnchorpointSourceControlSettingsWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.FillWidth(1.0f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(2.0f)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Install DirectoryLabel", "InstallDirectory"))
				.ToolTipText(LOCTEXT("InstallDirectoryLabel_Tooltip", "Path on disk to the InstallDirectory"))
			]
		]
		+SHorizontalBox::Slot()
		.FillWidth(2.0f)
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.FillHeight(1.0f)
			.Padding(2.0f)
			[
				SNew(SEditableTextBox)
				.Text(this, &SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText)
				.ToolTipText(LOCTEXT("InstallDirectoryLabel_Tooltip", "Path on disk to the InstallDirectory"))
				.OnTextCommitted(this, &SAnchorpointSourceControlSettingsWidget::OnInstallDirectoryTextCommited)
				.OnTextChanged(this, &SAnchorpointSourceControlSettingsWidget::OnInstallDirectoryTextCommited, ETextCommit::Default)
			]
		]
	];
}

FText SAnchorpointSourceControlSettingsWidget::GetInstallDirectoryText() const
{
	FAnchorpointSourceControlSettings& Settings = FAnchorpointModule::Get().GetSettings();
	return FText::FromString(Settings.GetInstallDirectory());
}

void SAnchorpointSourceControlSettingsWidget::OnInstallDirectoryTextCommited(const FText& Text, ETextCommit::Type Arg)
{
	FAnchorpointSourceControlSettings& Settings = FAnchorpointModule::Get().GetSettings();
	Settings.SetInstallDirectory(Text.ToString());
	Settings.SaveSettings();
}

#undef LOCTEXT_NAMESPACE