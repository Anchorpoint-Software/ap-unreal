// Copyright Epic Games, Inc. All Rights Reserved.

#include "Anchorpoint.h"

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
	// Register our operations
	AnchorpointSourceControlProvider.RegisterWorker("Connect", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointConnectWorker>));
	AnchorpointSourceControlProvider.RegisterWorker("CheckOut", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointCheckOutWorker>));
	AnchorpointSourceControlProvider.RegisterWorker("UpdateStatus", FAnchorpointSourceControlProvider::FGetAnchorpointSourceControlWorker::CreateStatic(&CreateWorker<FAnchorpointUpdateStatusWorker>));

	IModularFeatures::Get().RegisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);
}

void FAnchorpointModule::ShutdownModule()
{
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

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAnchorpointModule, Anchorpoint)
