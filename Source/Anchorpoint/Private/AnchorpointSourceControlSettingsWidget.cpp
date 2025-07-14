// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "AnchorpointSourceControlSettingsWidget.h"

#include "AnchorpointCli.h"
#include "AnchorpointCliOperations.h"

void SAnchorpointSourceControlSettingsWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.Padding(FMargin(0.0f, 0.0f, 16.0f, 16.0f))
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.Warning"))
				.Visibility_Lambda([this]() { return GetInstallationState() != EAnchorpointInstallationState::Ready ? EVisibility::Visible : EVisibility::Collapsed; })
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(SImage)
				.Image(FAppStyle::Get().GetBrush("Icons.Lock"))
				.Visibility_Lambda([this]() { return GetInstallationState() == EAnchorpointInstallationState::Ready ? EVisibility::Visible : EVisibility::Collapsed; })
			]
		]
		+ SHorizontalBox::Slot()
		.FillWidth(4.0f)
		.Padding(FMargin(0.0f, 0.0f, 16.0f, 16.0f))
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Anchorpoint", "AnchorpointNotInstalledWarning", "Anchorpoint’s desktop application is not available. You need to install and launch it first to continue."))
				.WrapTextAt(300.0f)
				.Visibility_Lambda([this]() { return GetInstallationState() == EAnchorpointInstallationState::NotInstalledOnDesktop ? EVisibility::Visible : EVisibility::Collapsed; })
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(NSLOCTEXT("Anchorpoint", "CheckDocumentation", "Check Documentation"))
					.Visibility_Lambda([this]() { return GetInstallationState() == EAnchorpointInstallationState::NotInstalledOnDesktop ? EVisibility::Visible : EVisibility::Collapsed; })
					.OnClicked_Lambda([]()
					{
						FPlatformProcess::LaunchURL(TEXT("https://docs.anchorpoint.app/docs/version-control/first-steps/unreal/"), nullptr, nullptr);
						return FReply::Handled();
					})
				]
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Anchorpoint", "AnchorpointNotInitializedWarning", "Open Anchorpoint to create a project for revision control."))
				.WrapTextAt(300.0f)
				.Visibility_Lambda([this]() { return GetInstallationState() == EAnchorpointInstallationState::NotInstalledInProject ? EVisibility::Visible : EVisibility::Collapsed; })
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(SButton)
					.Text(NSLOCTEXT("Anchorpoint", "OpenAnchorpoint", "Open Anchorpoint"))
					.Visibility_Lambda([this]() { return GetInstallationState() == EAnchorpointInstallationState::NotInstalledInProject ? EVisibility::Visible : EVisibility::Collapsed; })
					.OnClicked_Lambda([]()
					{
						const FString ProjectDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
						AnchorpointCliOperations::ShowInAnchorpoint(ProjectDir);
						return FReply::Handled();
					})
				]
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("Anchorpoint", "UnrealFileLockingWarning", "File locking will be controlled by the Unreal Editor. Keep the Anchorpoint project open for faster change detection."))
				.WrapTextAt(300.0f)
				.Visibility_Lambda([this]() { return GetInstallationState() == EAnchorpointInstallationState::Ready ? EVisibility::Visible : EVisibility::Collapsed; })
			]
		]
	];
}

EAnchorpointInstallationState SAnchorpointSourceControlSettingsWidget::GetInstallationState() const
{
	const FString InstallPath = FAnchorpointCliModule::Get().GetInstallFolder();
	if (InstallPath.IsEmpty() || !FPaths::DirectoryExists(InstallPath))
	{
		return EAnchorpointInstallationState::NotInstalledOnDesktop;
	}

	FString RepositoryRootPath = AnchorpointCliOperations::GetRepositoryRootPath();
	if (RepositoryRootPath.IsEmpty())
	{
		return EAnchorpointInstallationState::NotInstalledInProject;
	}

	return EAnchorpointInstallationState::Ready;
}