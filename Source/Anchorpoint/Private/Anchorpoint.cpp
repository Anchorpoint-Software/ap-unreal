// Copyright Epic Games, Inc. All Rights Reserved.

#include "Anchorpoint.h"

#include "AnchorpointLog.h"
#include "AnchorpointLogger.h"
#include "AnchorpointSouceControlOperations.h"

#include "apsync/version.h"
#include "apsync/service/api.h"
#include "apsync/launcher.h"

#include "Misc/Paths.h"
#include "HAL/PlatformMisc.h"

#include <memory>
#include "UObject/UObjectBaseUtility.h"

#include "apsync/projects/projects_api.h"
#include "apsync/account/account_api.h"
#include "apsync/locks/locks_api.h"
#include "apsync/service/object_api.h"

#include "GenericPlatform/GenericPlatformMisc.h"
#undef GetEnvironmentVariable

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

	Logger = std::make_shared<FAnchorpointLogger>();
	apsync::Api::setLogger(Logger);

	return;
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
     	FString ErrorMessage = StdStringToFString(ApiResult.error().message());
         UE_LOG(LogAnchorpoint, Error, TEXT("Failed to create API: %s"), *ErrorMessage);
         return;
     }
    
     _Api = ApiResult.value();
     auto Account = StdStringToFString(_Api->getAccount());
     UE_LOG(LogAnchorpoint, Log, TEXT("Logged in as: %s"), *Account);
    
     _SubscriptionHandler = NewObject<USubscription>();
     _SubscriptionHandler->Init(_Api);
     _SubscriptionHandler->AddToRoot();
     
     // Examples
     _SubscriptionHandler->RequestPush();
     ApiExample();
}

void FAnchorpointModule::ShutdownModule()
{
	AnchorpointSourceControlProvider.Close();
	IModularFeatures::Get().UnregisterModularFeature("SourceControl", &AnchorpointSourceControlProvider);

	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
     UE_LOG(LogAnchorpoint, Log, TEXT("Shutting down Anchorpoint plugin"));
	if(_SubscriptionHandler)
	{
     _SubscriptionHandler->RemoveFromRoot();
	}
}

void FAnchorpointModule::ApiExample() const {
    // Just some examples how to do things using apsync
    
    // Create our APIs
    auto ProjectsApi = std::make_shared<apsync::ProjectsApi>(_Api);
    auto AccountApi = std::make_shared<apsync::AccountApi>(_Api);
    auto LocksApi = std::make_shared<apsync::LocksApi>(_Api);
    auto ObjectsApi = std::make_shared<apsync::ObjectApi>(_Api);

    FString RelativeProjectDir = FPaths::ProjectDir();
    FString ProjectDir = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativeProjectDir);
    UE_LOG(LogAnchorpoint, Log, TEXT("Current Project Path: %s"), *ProjectDir);

    // Load the Anchorpoint Project
    auto ProjectDirStd = FStringToStdString(ProjectDir);
    auto ProjectResult = ProjectsApi->getProjectByPath(ProjectDirStd);
    if (ProjectResult.has_error()) {
        UE_LOG(LogAnchorpoint, Error, TEXT("Error loading Anchorpoint Project"));
    }

    std::string WorkspaceID = "";

    auto ProjectOptional = ProjectResult.value();
    if (!ProjectOptional.has_value()) {
        UE_LOG(LogAnchorpoint, Warning, TEXT("No Anchorpoint Project Found"));

        // No project found, so we will use the authenticated users private workspace
        auto UserResult = AccountApi->getUser(true);
        if (UserResult.has_error()) {
            UE_LOG(LogAnchorpoint, Error, TEXT("Error loading Anchorpoint User"));
            return;
        }

        auto User = UserResult.value();
        WorkspaceID = User.getUserspaceId();
    }
    else {
        // We found a project, use the workspace ID of that project
        auto Project = ProjectOptional.value();
        WorkspaceID = Project.workspaceId;
    }

    auto FWorkspaceID = StdStringToFString(WorkspaceID);
    UE_LOG(LogAnchorpoint, Log, TEXT("Workspace ID: %s"), *FWorkspaceID);

    // Handle Locks, return if ProjectOptional is not set as Locks only exist within Projects
    if (!ProjectOptional.has_value()) {
        return;
    }

    auto Project = ProjectOptional.value();

    // Get all active file locks of the workspace or project
    auto LocksResult = LocksApi->getLocks(WorkspaceID, ProjectOptional);
    if (LocksResult.has_error()) {
        UE_LOG(LogAnchorpoint, Error, TEXT("Error loading Anchorpoint Locks"));
        return;
    }
        
    auto Locks = LocksResult.value();
    for (const auto& Lock : Locks) {
        auto PathResult = LocksApi->getLockedPath(Lock, Project);
        if (PathResult.has_error()) {
            UE_LOG(LogAnchorpoint, Error, TEXT("Error loading Anchorpoint Locked Path"));
            continue;
        }
        auto Path = PathResult.value();
        auto LockedPath = StdStringToFString(Path);
        UE_LOG(LogAnchorpoint, Log, TEXT("Path Locked: %s"), *LockedPath);
    }

    // Example how to lock a file
    // In Anchorpoint, a file (or folder) needs to be uniquely identified before we can work with it
    std::string ExampleFilePath = "some file path";
    auto FExampleFilePath = StdStringToFString(ExampleFilePath);

    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FExampleFilePath)) {
        auto FileObject = ObjectsApi->identifyFile(ExampleFilePath, WorkspaceID, Project) err_ignore; // don't just ignore errors

        // Metadata is telling Anchorpoint it's locked by Git so that it automatically gets unlocked after changes have been pushed
        LocksApi->lock({ FileObject }, Project, std::nullopt, { {"type", "git"} }) err_ignore;
    }
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAnchorpointModule, Anchorpoint)
