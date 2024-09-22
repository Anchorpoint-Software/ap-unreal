// Copyright Epic Games, Inc. All Rights Reserved.

#include "Anchorpoint.h"

#include <ContentBrowserDataMenuContexts.h>
#include <ISourceControlModule.h>

#include "AnchorpointCli.h"
#include "AnchorpointSouceControlOperations.h"

#define LOCTEXT_NAMESPACE "FAnchorpointModule"

template <typename Type>
static TSharedRef<IAnchorpointSourceControlWorker> CreateWorker()
{
	return MakeShared<Type>();
}

void FAnchorpointModule::StartupModule()
{
	// Load settings for config
	AnchorpointSourceControlSettings.LoadSettings();

	// Connect the CLI to our settings to ensure we used the configured path 
	FAnchorpointCliModule::Get().GetCliPathDelegate.BindRaw(this, &FAnchorpointModule::GetCliPath);

	// Register our operations
	AnchorpointSourceControlProvider.RegisterWorker("Connect", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointConnectWorker>));
	AnchorpointSourceControlProvider.RegisterWorker("CheckOut", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointCheckOutWorker>));
	AnchorpointSourceControlProvider.RegisterWorker("Revert", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointRevertWorker>));
	AnchorpointSourceControlProvider.RegisterWorker("UpdateStatus", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointUpdateStatusWorker>));

	IModularFeatures::Get().RegisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);

	FToolMenuOwnerScoped OwnerScoped(this);
	if (UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("ContentBrowser.AssetContextMenu"))
	{
		Menu->AddDynamicSection(TEXT("AnchorpointDynamicSection"), FNewToolMenuDelegate::CreateRaw(this, &FAnchorpointModule::ExtendFileContextMenu));
	}
}

void FAnchorpointModule::ShutdownModule()
{
	UToolMenus::UnregisterOwner(this);
	
	AnchorpointSourceControlSettings.SaveSettings();

	AnchorpointSourceControlProvider.Close();
	IModularFeatures::Get().UnregisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);
}

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

FString FAnchorpointModule::GetCliPath()
{
	return GetSettings().GetInstallDirectory();
}

void FAnchorpointModule::ExtendFileContextMenu(UToolMenu* InMenu)
{
	ISourceControlProvider& CurrentProvider = ISourceControlModule::Get().GetProvider();
	if (CurrentProvider.GetName() != AnchorpointSourceControlProvider.GetName())
	{
		return;
	}

	UContentBrowserDataMenuContext_FileMenu* Context = InMenu->FindContext<UContentBrowserDataMenuContext_FileMenu>();
	FToolMenuSection& Section = InMenu->FindOrAddSection("AssetContextExploreMenuOptions");
	Section.AddMenuEntry(
		"ShowInAnchorpoint",
		LOCTEXT("ShowInAnchorpoint", "Show in Anchorpoint"),
		LOCTEXT("ShowInAnchorpointTip", "Show in Anchorpoint"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "ContentBrowser.ShowInExplorer"),
		FUIAction(
			FExecuteAction::CreateLambda([]()
			{
			})
		)
	);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAnchorpointModule, Anchorpoint)