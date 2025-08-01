// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "Anchorpoint.h"

#include <AssetDefinitionRegistry.h>
#include <ContentBrowserMenuContexts.h>
#include <ContentBrowserDataMenuContexts.h>
#include <ISourceControlModule.h>
#include <SourceControlHelpers.h>
#include <RevisionControlStyle/RevisionControlStyle.h>

#include "AnchorpointCli.h"
#include "AnchorpointCliOperations.h"
#include "AnchorpointSourceControlOperations.h"
#include "AnchorpointStyle.h"

FAnchorpointModule& FAnchorpointModule::Get()
{
	return FModuleManager::GetModuleChecked<FAnchorpointModule>("Anchorpoint");
}

FAnchorpointSourceControlProvider& FAnchorpointModule::GetProvider()
{
	return AnchorpointSourceControlProvider;
}

FAnchorpointSourceControlSettings& FAnchorpointModule::GetSettings()
{
	return AnchorpointSourceControlSettings;
}


void FAnchorpointModule::StartupModule()
{
	// Style setup

	// NOTE: Unreal's recommended way to style/customize RevisionControl Styles is to inherit from FDefaultRevisionControlStyle
	// However, someone forgot to DLL export it, so using it leads to linking errors.
	//
	// As a workaround, Anchorpoint will replace the Active Revision Control Style but keep a reference to the default one.
	// When an icon is requested, Anchorpoint will try to serve it, if failed, it will use the default revision control style as a fallback.
	FAnchorpointStyle& AnchorpointStyle = FAnchorpointStyle::Get();

	const FName Default = FRevisionControlStyleManager::GetStyleSetName();
	AnchorpointStyle.SetFallbackStyleName(Default);

	const FName AnchorpointStyleName = AnchorpointStyle.GetStyleSetName();
	FRevisionControlStyleManager::SetActiveRevisionControlStyle(AnchorpointStyleName);

	// Load settings for config
	AnchorpointSourceControlSettings.LoadSettings();

	// Register our operations
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointConnectWorker>("Connect");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointCheckOutWorker>("CheckOut");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointRevertWorker>("Revert");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointUpdateStatusWorker>("UpdateStatus");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointAddWorker>("MarkForAdd");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointDeleteWorker>("Delete");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointCopyWorker>("Copy");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointCheckInWorker>("CheckIn");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointDownloadFileWorker>("DownloadFile");
	AnchorpointSourceControlProvider.RegisterWorker<FAnchorpointResolveWorker>("Resolve");

	IModularFeatures::Get().RegisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);

	FToolMenuOwnerScoped OwnerScoped(this);
	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu"))
	{
		Menu->AddDynamicSection(TEXT("AnchorpointDynamicSection"), FNewToolMenuDelegate::CreateRaw(this, &FAnchorpointModule::ExtendFileContextMenu));
	}

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		if(TSoftClassPtr<UObject> AssetSoftClass = *ClassIt)
		{
			FNameBuilder Builder;
			Builder << TEXT("ContentBrowser.AssetContextMenu.");
			Builder << AssetSoftClass->GetFName();
			Builder << TEXT(".SourceControlSubMenu");

			const FName MenuName(Builder.ToView());

			if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(MenuName))
			{
				FToolMenuSection& Section = Menu->AddSection("Anchorpoint", NSLOCTEXT("Anchorpoint", "Anchorpoint", "Anchorpoint"));
				Section.AddDynamicEntry("MergeInAnchorpointDynamic", FNewToolMenuSectionDelegate::CreateRaw(this, &FAnchorpointModule::RegisterMergeWithAnchorpoint, AssetSoftClass));
			}
		}
	}
}

void FAnchorpointModule::ShutdownModule()
{
	UToolMenus::UnregisterOwner(this);

	AnchorpointSourceControlSettings.SaveSettings();

	AnchorpointSourceControlProvider.Close();
	IModularFeatures::Get().UnregisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);
}

void FAnchorpointModule::ExtendFileContextMenu(UToolMenu* InMenu)
{
	FAnchorpointCliModule& CliModule = FAnchorpointCliModule::Get();
	const bool bUsesAnchorpoint = CliModule.IsCurrentProvider();

	if (!bUsesAnchorpoint)
	{
		return;
	}

	UContentBrowserDataMenuContext_FileMenu* Context = InMenu->FindContext<UContentBrowserDataMenuContext_FileMenu>();
	TArray<FContentBrowserItem> Items = Context->SelectedItems;

	FString Path;
	if (Items.Num() != 1 || !Items[0].GetItemPhysicalPath(Path))
	{
		return;
	}

	Path = FPaths::ConvertRelativePathToFull(Path);

	FToolMenuSection& Section = InMenu->FindOrAddSection("AssetContextExploreMenuOptions");
	Section.AddMenuEntry(
		"ShowInAnchorpoint",
		NSLOCTEXT("Anchorpoint", "ShowInAnchorpoint", "Show in Anchorpoint"),
		NSLOCTEXT("Anchorpoint", "ShowInAnchorpointTip", "Show in Anchorpoint"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ShowInExplorer"),
		FUIAction(
			FExecuteAction::CreateLambda([Path]()
			{
				AnchorpointCliOperations::ShowInAnchorpoint(Path);
			})
		)
	);
}

void FAnchorpointModule::RegisterMergeWithAnchorpoint(FToolMenuSection& InSection, TSoftClassPtr<> AssetSoftClass)
{
	UContentBrowserAssetContextMenuContext* const Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
	if (!Context || Context->CommonClass != AssetSoftClass)
	{
		return;
	}

	TArray<FString> Files;
	for (const FAssetData& AssetData : Context->SelectedAssets)
	{
		if (const UAssetDefinition* AssetDefinition = UAssetDefinitionRegistry::Get()->GetAssetDefinitionForAsset(AssetData))
		{
			if (AssetDefinition->CanMerge())
			{
				// Unreal is handling at least one of the selected asset type
				return;
			}
		}

		const FString Filename = USourceControlHelpers::PackageFilename(AssetData.PackageName.ToString());
		Files.Add(Filename);
	}

	TArray<FSourceControlStateRef> FileStates;
	ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();

	SourceControlProvider.GetState(Files, FileStates, EStateCacheUsage::Use);
	for (const FSourceControlStateRef& State : FileStates)
	{
		if (!State->IsConflicted())
		{
			return;
		}
	}

	if (FileStates.IsEmpty())
	{
		return;
	}

	InSection.AddMenuEntry(
		"MergeInAnchorpoint",
		NSLOCTEXT("Anchorpoint", "MergeInAnchorpoint", "Merge in Anchorpoint"),
		NSLOCTEXT("Anchorpoint", "MergeInAnchorpointTip", "Manually complete the merge in Anchorpoint using the merge UI helpers."),
		FSlateIcon("DefaultRevisionControlStyle", "RevisionControl.Actions.Merge"),
		FUIAction(
			FExecuteAction::CreateLambda([Files]()
			{
				if (Files.Num() == 1)
				{
					AnchorpointCliOperations::ShowInAnchorpoint(Files[0]);
				}
				else
				{
					AnchorpointCliOperations::ShowInAnchorpoint();
				}
			})
		));
}

IMPLEMENT_MODULE(FAnchorpointModule, Anchorpoint)