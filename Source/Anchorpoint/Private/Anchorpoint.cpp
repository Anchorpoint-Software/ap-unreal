#include "Anchorpoint.h"

#include <ContentBrowserDataMenuContexts.h>
#include <ISourceControlModule.h>

#include "AnchorpointCli.h"
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

	// Connect the CLI to our settings to ensure we used the configured path 
	FAnchorpointCliModule::Get().GetInstallFolderDelegate.BindRaw(this, &FAnchorpointModule::GetCliPath);

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
}

void FAnchorpointModule::ShutdownModule()
{
	UToolMenus::UnregisterOwner(this);

	AnchorpointSourceControlSettings.SaveSettings();

	AnchorpointSourceControlProvider.Close();
	IModularFeatures::Get().UnregisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);
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
	TArray<FContentBrowserItem> Items = Context->SelectedItems;

	FString Path;
	if(Items.Num() != 1 || !Items[0].GetItemPhysicalPath(Path))
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