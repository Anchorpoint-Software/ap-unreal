// Copyright Anchorpoint Software GmbH. All Rights Reserved.

#include "Subscription.h"
#include "TimerManager.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"

#include "Runtime/Core/Public/Misc/Paths.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "IImageWrapperModule.h"
#include "ObjectTools.h"
#include "IImageWrapper.h"
#include "Misc/FileHelper.h"
#include "PackageTools.h"
#include "Misc/PackageName.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Blueprint.h"
#include "Engine/BlueprintGeneratedClass.h"

#include "apsync/service/thumbnail/thumbnail_service.h"


DEFINE_LOG_CATEGORY(LogAnchorpointSync);

USubscription::~USubscription(){
    if (_IpcApi) {
        auto result = _IpcApi->unsubscribe(_IpcSubscription);
        if (result.has_error()) {
            UE_LOG(LogAnchorpointSync, Log, TEXT("Failed to unsubscribe from Anchorpoint IPC"));
        }
    }
}

void USubscription::Init(std::shared_ptr<apsync::Api> Api) {
    _Api = Api;
    
    _IpcApi = std::make_shared<apsync::IpcApi>(_Api);
    _ColorApi = std::make_shared<apsync::ColorApi>(_Api);
    _ProjectsApi = std::make_shared<apsync::ProjectsApi>(_Api);
    
    auto SubscriptionResult = _IpcApi->subscribe(_IpcTopic);
    if (SubscriptionResult.has_error()){
        UE_LOG(LogAnchorpointSync, Error, TEXT("Could not establish connection to Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(SubscriptionResult.error().message().c_str())));
    } else {
        UE_LOG(LogAnchorpointSync, Log, TEXT("Connected to Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(SubscriptionResult.value().getId().c_str())));
        _IpcSubscription = SubscriptionResult.value();
    }
}

bool USubscription::IsTickableInEditor() const
{
    // Return true if you want this object to be ticked, otherwise false.
    return true;
}

TStatId USubscription::GetStatId() const
{
    // Return a unique identifier for this tickable object (usually based on the class name).
    return TStatId();
}

void USubscription::Tick(float DeltaTime)
{
    // Implement your per-frame logic here.
    // This function will be called each frame by the engine.
    _ElapsedTime = _ElapsedTime + DeltaTime;
    if (_ElapsedTime > 1.0f) {
        _ElapsedTime = 0.0f;
        if (_IpcApi != nullptr) {
            while (_IpcApi->hasMessages(_IpcSubscription).value()) {
                auto MessageOptResult = _IpcApi->tryGetMessage(_IpcSubscription);
                if (MessageOptResult.has_error()){
                    UE_LOG(LogAnchorpointSync, Error, TEXT("Could not get message from Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(MessageOptResult.error().message().c_str())));
                } else {
                    auto MessageOpt = MessageOptResult.value();
                    if (MessageOpt && MessageOpt->getSenderId() != IpcSenderId) {
                        UE_LOG(LogAnchorpointSync, Log, TEXT("Received a message from Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(MessageOpt->getSenderId().c_str())));
                        HandleMessage(*MessageOpt);
                    }
                }
            }
        }
    }
}

void USubscription::RequestPush() {
    apsync::IpcMessage Request;

    Request.setKind("request-push");
    std::unordered_map<std::string, std::string> Header = { {"project_path","TODO"} };
    Request.setHeader(Header);

    UE_LOG(LogAnchorpointSync, Log, TEXT("Example: Request Push"));

    PublishMessage(Request);
}

void USubscription::PublishMessage(apsync::IpcMessage& Message) {
    Message.setTopic(_IpcTopic);
    
    auto result = _IpcApi->publish(Message);
    if (result.has_error()){
        UE_LOG(LogAnchorpointSync, Error, TEXT("Could not publish Anchorpoint IPC message: %s"), *FString(ANSI_TO_TCHAR(result.error().message().c_str())));
    }
}

void USubscription::HandleMessage(const apsync::IpcMessage& Message) {
    std::optional<apsync::IpcMessage> ResultOpt;
    if (Message.getKind() == "request-release-packages"){
        ResultOpt = HandleRequestReleasePackages(Message);
    }
    else {
        UE_LOG(LogAnchorpointSync, Error, TEXT("Could not handle Anchorpoint IPC message, unknown kind: %s"), *FString(ANSI_TO_TCHAR(Message.getKind().c_str())));
    }
    
    if (ResultOpt) {
        PublishMessage(*ResultOpt);
    }
}

std::optional<apsync::IpcMessage> USubscription::HandleRequestReleasePackages(const apsync::IpcMessage& Message) {
    // Pulling in Anchorpoint is not possible if Unreal Engine is running.
    // Unload packages similar to UnlinkPackages in Engine code
    // After that's done, let Anchorpoint know that we did so

    apsync::IpcMessage Response;
    Response.setKind("response-release-packages");
    
    std::unordered_map<std::string, std::string> Header = { {"success","true"} };
    Response.setHeader(Header);

    return Response;
}

//
//std::optional<apsync::IpcMessage> USubscription::HandleThumbnailRequest(const apsync::IpcMessage& Message){
//    apsync::IpcMessage Response;
//    Response.setKind("response-thumbnail");
//    
//    auto WorkspaceId = Message.getHeader().at("workspace_id");
//    auto ProjectId = Message.getHeader().at("project_id");
//    auto ProjectPath= Message.getHeader().at("project_path");
//    auto ProjectResult = _ProjectsApi->getProjectForPath({ProjectPath, true});
//    if (ProjectResult.has_error()) {
//        UE_LOG(LogAnchorpointSync, Error, TEXT("Invalid Project"));
//        std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Invalid Project"}};
//        Response.setHeader(header);
//        return Response;
//    }
//    auto Project = ProjectResult.value();
//    
//    auto JsonString = FString(UTF8_TO_TCHAR(Message.getBody().c_str()));
//    TArray<TSharedPtr<FJsonValue>> JsonValues;
//    
//    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
//    if (FJsonSerializer::Deserialize(Reader, JsonValues))
//    {
//        auto ColorConfigResult = _ColorApi->getConfig(WorkspaceId, Project);
//        if (ColorConfigResult.has_error()) {
//            UE_LOG(LogAnchorpointSync, Error, TEXT("Invalid Color Config"));
//            std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Color Config is Invalid"}};
//            Response.setHeader(header);
//            return Response;
//        }
//        
//        auto ColorConfigHashResult = _ColorApi->getConfigHash(ColorConfigResult.value());
//        if (ColorConfigHashResult.has_error()) {
//            UE_LOG(LogAnchorpointSync, Error, TEXT("Invalid Color Config Hash"));
//            std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Color Config Hash is Invalid"}};
//            Response.setHeader(header);
//            return Response;
//        }
//        auto ColorConfigHash = ColorConfigHashResult.value();
//        
//        TArray<FString> StringArray;
//        for (const TSharedPtr<FJsonValue>& Value : JsonValues)
//        {
//            FString Item = Value->AsString();
//            StringArray.Add(Item);
//            UE_LOG(LogAnchorpointSync, Log, TEXT("Received Path for Thumbnail: %s"), *Item);
//            
//            auto PreviewPathOpt = apsync::ThumbnailService::getPreviewImagePath(std::string(TCHAR_TO_UTF8(*Item)), ColorConfigHash);
//            if (!PreviewPathOpt){
//                UE_LOG(LogAnchorpointSync, Error, TEXT("Could not get preview path for: %s"), *Item);
//                continue;
//            } else {
//                UE_LOG(LogAnchorpointSync, Log, TEXT("Saving Thumbnail to: %s"), UTF8_TO_TCHAR(PreviewPathOpt->c_str()));
//            }
//            
//            SaveThumbnail(Item, FString(UTF8_TO_TCHAR(PreviewPathOpt->c_str())));
//        }
//        
//        std::unordered_map<std::string, std::string> header = {{"success","true"}};
//        Response.setHeader(header);
//        return Response;
//    } else {
//        UE_LOG(LogAnchorpointSync, Error, TEXT("Could not parse request json"));
//        std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Could not parse request body"}};
//        Response.setHeader(header);
//        return Response;
//    }
//    
//    return std::nullopt;
//}
