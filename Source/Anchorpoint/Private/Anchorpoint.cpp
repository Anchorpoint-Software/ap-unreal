// Copyright Epic Games, Inc. All Rights Reserved.

#include "Anchorpoint.h"

#include "AnchorpointCli.h"
#include "AnchorpointLog.h"
#include "AnchorpointSouceControlOperations.h"

#define LOCTEXT_NAMESPACE "FAnchorpointModule"

template<typename Type>
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
	AnchorpointSourceControlProvider.RegisterWorker("UpdateStatus", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointUpdateStatusWorker>));

	IModularFeatures::Get().RegisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);
}

void FAnchorpointModule::ShutdownModule()
{
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAnchorpointModule, Anchorpoint)
