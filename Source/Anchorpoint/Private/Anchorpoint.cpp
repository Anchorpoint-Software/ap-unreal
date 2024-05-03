// Copyright Epic Games, Inc. All Rights Reserved.

#include "Anchorpoint.h"
#include "apsync/version.h"
#include "apsync/service/api.h"
#include "apsync/launcher.h"

#include "Misc/Paths.h"
#include "HAL/PlatformMisc.h"

#include <memory>
#include "UObject/UObjectBaseUtility.h"

#define LOCTEXT_NAMESPACE "FAnchorpointModule"

using namespace std;

std::string FStringToStdString(const FString& UnrealString)
{
    // Convert FString to UTF-8 using FTCHARToUTF8
    FTCHARToUTF8 Converter(*UnrealString);  // Pass the FString data to the converter

    // Construct a std::string with the converted data
    return std::string(Converter.Get(), Converter.Length());
}

FString StdStringToFString(const string& StdString)
{
    return UTF8_TO_TCHAR(StdString.c_str());
}

void FAnchorpointModule::StartupModule()
{
	// This will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    UE_LOG(LogAnchorpoint, Log, TEXT("Hello from Anchorpoint Plugin: %s"), *FString(AP_VERSION));
    
    // The name of the environment variable you want to read
    const TCHAR* APExeEnv = TEXT("AP_EXE");
    const TCHAR* APDExeEnv = TEXT("APD_EXE");

    // Get the environment variable value
    FString APExeEnvVarValue = FPlatformMisc::GetEnvironmentVariable(APExeEnv);
    FString APDExeEnvVarValue = FPlatformMisc::GetEnvironmentVariable(APDExeEnv);
    
    optional<string> PathToAnchorpoint = nullopt;
    if (!APExeEnvVarValue.IsEmpty())
    {
        UE_LOG(LogAnchorpoint, Log, TEXT("Using Custom Anchorpoint: %s"), *APExeEnvVarValue);
        PathToAnchorpoint = FStringToStdString(APExeEnvVarValue);
    }
    
    if (!APDExeEnvVarValue.IsEmpty())
    {
        UE_LOG(LogAnchorpoint, Log, TEXT("Using Custom Daemon Path: %s"), *APDExeEnvVarValue);
        auto PathToAnchorpointDaemon = FStringToStdString(APDExeEnvVarValue);
        apsync::DaemonLauncher::setDaemonPath(PathToAnchorpointDaemon);
    }

    apsync::Api::startAnchorpoint(PathToAnchorpoint, true) err_ignore;

    auto ApiResult = apsync::Api::createFromAuthenticatedUser(IpcSenderId);
    if (!ApiResult) {
        UE_LOG(LogAnchorpoint, Error, TEXT("Failed to create API: %s"), UTF8_TO_TCHAR(ApiResult.error().message().c_str()));
        return;
    }
    
    auto Api = ApiResult.value();
    auto Account = StdStringToFString(Api->getAccount());
    UE_LOG(LogAnchorpoint, Log, TEXT("Logged in as: %s"), *Account);
    
    _SubscriptionHandler = NewObject<USubscription>();
    _SubscriptionHandler->Init(Api);
    _SubscriptionHandler->AddToRoot();
}

void FAnchorpointModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    UE_LOG(LogAnchorpoint, Log, TEXT("Shutting down Anchorpoint plugin"));
    _SubscriptionHandler->RemoveFromRoot();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAnchorpointModule, Anchorpoint)
