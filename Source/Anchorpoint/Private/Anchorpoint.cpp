// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#include "Anchorpoint.h"

#include <ContentBrowserMenuContexts.h>
#include <ContentBrowserDataMenuContexts.h>
#include <ISourceControlModule.h>

#include "AnchorpointCliOperations.h"
#include "AnchorpointSourceControlOperations.h"

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

	IModularFeatures::Get().RegisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);

	FToolMenuOwnerScoped OwnerScoped(this);
	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu"))
	{
		Menu->AddDynamicSection(TEXT("AnchorpointDynamicSection"), FNewToolMenuDelegate::CreateRaw(this, &FAnchorpointModule::ExtendFileContextMenu));
	}

	for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
	{
		TSoftClassPtr<UObject> AssetSoftClass = *ClassIt;
		FNameBuilder Builder;
		Builder << TEXT("ContentBrowser.AssetContextMenu.");
		Builder << AssetSoftClass->GetFName();
		Builder << TEXT(".SourceControlSubMenu");

		const FName MenuName(Builder.ToView());

		if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu(MenuName))
		{
			//TODO: 1) only show if the asset in a conflicted state and the built-in merge is not possible
			//TODO: 2) Complete the callback to open the asset in anchorpoint

			FToolMenuSection& Section = Menu->AddSection("Anchorpoint", NSLOCTEXT("Anchorpoint", "Anchorpoint", "Anchorpoint"));
			Section.AddDynamicEntry("MergeInAnchorpointDynamic",
			                        FNewToolMenuSectionDelegate::CreateLambda([AssetSoftClass](FToolMenuSection& InSection)
			                        {
				                        UContentBrowserAssetContextMenuContext* const Context = InSection.FindContext<UContentBrowserAssetContextMenuContext>();
				                        if (!Context || Context->CommonClass != AssetSoftClass)
				                        {
					                        return;
				                        }

				                        InSection.AddMenuEntry(
					                        "MergeInAnchorpoint",
					                        NSLOCTEXT("Anchorpoint", "MergeInAnchorpoint", "Merge in Anchorpoint"),
					                        NSLOCTEXT("Anchorpoint", "MergeInAnchorpointTip", "Manually complete the merge in Anchorpoint using the merge UI helpers."),
					                        FSlateIcon("DefaultRevisionControlStyle", "RevisionControl.Actions.Merge"),
					                        FUIAction(
						                        FExecuteAction::CreateLambda([AssetSoftClass]()
						                        {
						                        })
					                        ));
			                        }));
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
	ISourceControlProvider& CurrentProvider = ISourceControlModule::Get().GetProvider();
	if (CurrentProvider.GetName() != AnchorpointSourceControlProvider.GetName())
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

IMPLEMENT_MODULE(FAnchorpointModule, Anchorpoint)