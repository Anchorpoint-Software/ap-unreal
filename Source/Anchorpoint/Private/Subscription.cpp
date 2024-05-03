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


DEFINE_LOG_CATEGORY(LogAnchorpoint);

USubscription::~USubscription(){
    _IpcApi->unsubscribe(_IpcSubscription);
}

void USubscription::Init(std::shared_ptr<apsync::Api> Api) {
    _Api = Api;
    
    _IpcApi = std::make_shared<apsync::IpcApi>(_Api);
    _ColorApi = std::make_shared<apsync::ColorApi>(_Api);
    _ProjectsApi = std::make_shared<apsync::ProjectsApi>(_Api);
    
    auto SubscriptionResult = _IpcApi->subscribe(_IpcTopic);
    if (SubscriptionResult.has_error()){
        UE_LOG(LogAnchorpoint, Error, TEXT("Could not establish connection to Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(SubscriptionResult.error().message().c_str())));
    } else {
        UE_LOG(LogAnchorpoint, Log, TEXT("Connected to Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(SubscriptionResult.value().getId().c_str())));
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
                    UE_LOG(LogAnchorpoint, Error, TEXT("Could not get message from Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(MessageOptResult.error().message().c_str())));
                } else {
                    auto MessageOpt = MessageOptResult.value();
                    if (MessageOpt && MessageOpt->getSenderId() != IpcSenderId) {
                        UE_LOG(LogAnchorpoint, Log, TEXT("Received a message from Anchorpoint: %s"), *FString(ANSI_TO_TCHAR(MessageOpt->getSenderId().c_str())));
                        HandleMessage(*MessageOpt);
                    }
                }
            }
        }
    }
}

void USubscription::PublishMessage(apsync::IpcMessage& Message){
    Message.setTopic(_IpcTopic);
    
    auto result = _IpcApi->publish(Message);
    if (result.has_error()){
        UE_LOG(LogAnchorpoint, Error, TEXT("Could not publish Anchorpoint IPC message: %s"), *FString(ANSI_TO_TCHAR(result.error().message().c_str())));
    }
}

void USubscription::HandleMessage(const apsync::IpcMessage& Message){
    std::optional<apsync::IpcMessage> ResultOpt;
    if (Message.getKind() == "request-thumbnail"){
        ResultOpt = HandleThumbnailRequest(Message);
    }
    else {
        UE_LOG(LogAnchorpoint, Error, TEXT("Could not handle Anchorpoint IPC message, unknown kind: %s"), *FString(ANSI_TO_TCHAR(Message.getKind().c_str())));
    }
    
    if (ResultOpt) {
        PublishMessage(*ResultOpt);
    }
}

std::optional<apsync::IpcMessage> USubscription::HandleThumbnailRequest(const apsync::IpcMessage& Message){
    apsync::IpcMessage Response;
    Response.setKind("response-thumbnail");
    
    auto WorkspaceId = Message.getHeader().at("workspace_id");
    auto ProjectId = Message.getHeader().at("project_id");
    auto ProjectPath= Message.getHeader().at("project_path");
    auto ProjectResult = _ProjectsApi->getProjectForPath({ProjectPath, true});
    if (ProjectResult.has_error()) {
        UE_LOG(LogAnchorpoint, Error, TEXT("Invalid Project"));
        std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Invalid Project"}};
        Response.setHeader(header);
        return Response;
    }
    auto Project = ProjectResult.value();
    
    auto JsonString = FString(UTF8_TO_TCHAR(Message.getBody().c_str()));
    TArray<TSharedPtr<FJsonValue>> JsonValues;
    
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (FJsonSerializer::Deserialize(Reader, JsonValues))
    {
        auto ColorConfigResult = _ColorApi->getConfig(WorkspaceId, Project);
        if (ColorConfigResult.has_error()) {
            UE_LOG(LogAnchorpoint, Error, TEXT("Invalid Color Config"));
            std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Color Config is Invalid"}};
            Response.setHeader(header);
            return Response;
        }
        
        auto ColorConfigHashResult = _ColorApi->getConfigHash(ColorConfigResult.value());
        if (ColorConfigHashResult.has_error()) {
            UE_LOG(LogAnchorpoint, Error, TEXT("Invalid Color Config Hash"));
            std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Color Config Hash is Invalid"}};
            Response.setHeader(header);
            return Response;
        }
        auto ColorConfigHash = ColorConfigHashResult.value();
        
        TArray<FString> StringArray;
        for (const TSharedPtr<FJsonValue>& Value : JsonValues)
        {
            FString Item = Value->AsString();
            StringArray.Add(Item);
            UE_LOG(LogAnchorpoint, Log, TEXT("Received Path for Thumbnail: %s"), *Item);
            
            auto PreviewPathOpt = apsync::ThumbnailService::getPreviewImagePath(std::string(TCHAR_TO_UTF8(*Item)), ColorConfigHash);
            if (!PreviewPathOpt){
                UE_LOG(LogAnchorpoint, Error, TEXT("Could not get preview path for: %s"), *Item);
                continue;
            } else {
                UE_LOG(LogAnchorpoint, Log, TEXT("Saving Thumbnail to: %s"), UTF8_TO_TCHAR(PreviewPathOpt->c_str()));
            }
            
            SaveThumbnail(Item, FString(UTF8_TO_TCHAR(PreviewPathOpt->c_str())));
        }
        
        std::unordered_map<std::string, std::string> header = {{"success","true"}};
        Response.setHeader(header);
        return Response;
    } else {
        UE_LOG(LogAnchorpoint, Error, TEXT("Could not parse request json"));
        std::unordered_map<std::string, std::string> header = {{"success","false"}, {"message","Could not parse request body"}};
        Response.setHeader(header);
        return Response;
    }
    
    return std::nullopt;
}

FString GetPathFromPackage(FString PackagePath) //path to uasset
{
    // TODO:
    // Convert Path to Unreal Path

    /*FString result;
    FString failureReason;
    if (!FPackageName::TryConvertFilenameToLongPackageName(PackagePath, result, &failureReason)){
        UE_LOG(LogAnchorpoint, Warning, TEXT("Error: %s"), *failureReason)
        return PackagePath;
    }
    
    return result;*/
    
    auto MyPackage = UPackageTools::LoadPackage(PackagePath);
    if (MyPackage)
    {
        TArray<UObject*> ObjectsInPackage;
        GetObjectsWithPackage(MyPackage, ObjectsInPackage);
        for (UObject* PotentialActor : ObjectsInPackage)
        {
            auto ClassName = PotentialActor->GetClass()->GetName();
            
            auto ActorCast = Cast<AActor>(PotentialActor);
            if (ActorCast)
            {
                auto Name = ActorCast->GetActorNameOrLabel();
                UE_LOG(LogAnchorpoint, Warning, TEXT("ActorName: %s"), *Name);
            }

            // Gets Unreal Path Name
            auto UPathName = GetPathNameSafe(PotentialActor);
            
            // returns Pathname if the class is a StaticMesh or a Blueprint
            if (ClassName == "StaticMesh" || ClassName == "Blueprint") {
                UE_LOG(LogAnchorpoint, Warning, TEXT("ClassName: %s"), *ClassName);
                UE_LOG(LogAnchorpoint, Warning, TEXT("UName: %s"), *UPathName);
                return UPathName;
            }
        }
    }
    // if the class is none of the above, then return false -> doesnt generate thumbnail
    return "False";
}

FString ConvertSystemPath(const FString& SystemPath) {
    FString AssetPath = SystemPath;
    FString ContentDir = FPaths::ProjectContentDir();
    FPaths::MakePathRelativeTo(AssetPath, *ContentDir);
    UE_LOG(LogAnchorpoint, Log, TEXT("ContentDir: %s"), *ContentDir);
    AssetPath = TEXT("/Game/") + AssetPath;
    AssetPath.RemoveFromEnd(TEXT(".uasset"));
    AssetPath.RemoveFromEnd(TEXT(".umap"));
    return AssetPath;
}

bool SaveThumbnailToDisk(FObjectThumbnail* Thumbnail, FString OutputPath) {
    if (Thumbnail == nullptr)
    {
       return false;
    }
    
    // Convert the thumbnail data to a raw image format
    auto RawImageData = Thumbnail->GetUncompressedImageData();

    // Ensure the image wrapper module is loaded
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

    if (!ImageWrapper.IsValid())
    {
       return false;
    }

    //Set the raw image data
    if (!ImageWrapper->SetRaw(RawImageData.GetData(), RawImageData.Num(), Thumbnail->GetImageWidth(), Thumbnail->GetImageHeight(), ERGBFormat::BGRA, 8))
    {
       return false;
    }

    // Save the image data to a PNG file
    const TArray64<uint8>& CompressedByteArray = ImageWrapper->GetCompressed();
    if (FFileHelper::SaveArrayToFile(CompressedByteArray, *OutputPath))
    {
        UE_LOG(LogAnchorpoint, Log, TEXT("Save Completed"));
        return true;
    }
    

    return false;
}

//const bool bUsesGenericThumbnail = [ObjectBeingModified, this]() -> bool                        // No need to dirty generic thumbnails
//{
//    if (FThumbnailRenderingInfo* RenderingInfo = GetRenderingInfo(ObjectBeingModified))
//    {
//        return RenderingInfo->Renderer == nullptr;
//    }
//    else
//    {
//        return true;
//    }
//}();

AActor* GetOFPA(const FString& ActorFilePath) {
    auto Package = UPackageTools::LoadPackage(ActorFilePath);
    if (!Package)
    {
        return nullptr;
    }
    
    TArray<UObject*> ObjectsInPackage;
    GetObjectsWithPackage(Package, ObjectsInPackage);
    bool ObjectFound = false;
    for (UObject* Object : ObjectsInPackage)
    {
        if (AActor* Actor = Cast<AActor>(Object)) {
            return Actor;
        }
    }
    
    return nullptr;
}

bool GetAssetFromActor(const FString& ActorFilePath, FString& OutAssetPath) {
    auto Actor = GetOFPA(ActorFilePath);
    if (!Actor) {
        return false;
    }
    
    auto GetAssetPath = [](const FAssetData& AssetData){
        auto PackagePath = AssetData.GetPackage()->GetLoadedPath();
        return FPaths::ConvertRelativePathToFull(PackagePath.GetLocalFullPath());
    };
    
    // See UEditorEngine::GetAssetsToSyncToContentBrowser
    const FString& BrowseToAssetOverride = Actor->GetBrowseToAssetOverride();
    if (!BrowseToAssetOverride.IsEmpty()) {
        if (IAssetRegistry* AssetRegistry = IAssetRegistry::Get())
        {
            TArray<FAssetData> FoundAssets;
            if (AssetRegistry->GetAssetsByPackageName(*BrowseToAssetOverride, FoundAssets) && FoundAssets.Num() > 0) {
                OutAssetPath = GetAssetPath(FoundAssets[0]);
                return true;
            }
        }
    }
    
    // If the actor is an instance of a blueprint, just add the blueprint.
    UBlueprint* GeneratingBP = Cast<UBlueprint>(Actor->GetClass()->ClassGeneratedBy);
    if (GeneratingBP != NULL)
    {
        OutAssetPath = GetAssetPath(FAssetData(GeneratingBP));
        return true;
    }
    // Cooked editor sometimes only contains UBlueprintGeneratedClass with no UBlueprint
    else if (UBlueprintGeneratedClass* BlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(Actor->GetClass()))
    {
        OutAssetPath = GetAssetPath(FAssetData(BlueprintGeneratedClass));
        return true;
    }
    // Otherwise, add the results of the GetReferencedContentObjects call
    else
    {
        TArray<UObject*> Objects;
        Actor->GetReferencedContentObjects(Objects);
        for (UObject* Object : Objects)
        {
            UE_LOG(LogAnchorpoint, Log, TEXT("GetReferencedContentObjects: %s"), *Object->GetFullName());
            OutAssetPath = GetAssetPath(FAssetData(Object));
            return true;
        }

        TArray<FSoftObjectPath> SoftObjects;
        Actor->GetSoftReferencedContentObjects(SoftObjects);

        if (SoftObjects.Num())
        {
            IAssetRegistry& AssetRegistry = IAssetRegistry::GetChecked();

            for (const FSoftObjectPath& SoftObject : SoftObjects)
            {
                FAssetData AssetData = AssetRegistry.GetAssetByObjectPath(SoftObject);

                if (AssetData.IsValid())
                {
                    OutAssetPath = GetAssetPath(FAssetData(AssetData));
                    return true;
                }
            }
        }
    }
    
    return false;
}

// Returns true when the uasset on disk is within __ExternalActors__
bool IsUAssetOFPA(const FString& PathToFileOnDisk)
{
    // look for the __External_Actors__ Folder because the OFPA Files are saved in this folder
    if (PathToFileOnDisk.Contains(FPackagePath::GetExternalActorsFolderName())) {
        return true;
    }
    return false;
}

bool USubscription::SaveThumbnail(FString FilePath, FString OutputPath)
{
    if (IsUAssetOFPA(FilePath)) {
        if (!GetAssetFromActor(FilePath, FilePath)){
            return false;
        }
    }

    FString AssetPath = ConvertSystemPath(FilePath);
    UE_LOG(LogAnchorpoint, Log, TEXT("AssetPath: %s"), *AssetPath);
    
    FObjectThumbnail* ObjectThumbnail = nullptr;
    
    UObject* Asset = StaticLoadObject(UObject::StaticClass(), nullptr, *AssetPath);
    if (Asset) {
        auto AssetName = FName(*Asset->GetFullName());
        UE_LOG(LogAnchorpoint, Log, TEXT("AssetName: %s"), *Asset->GetFullName());
        ObjectThumbnail = ThumbnailTools::GetThumbnailForObject(Asset);
        
        if (!ObjectThumbnail){
            FThumbnailMap LoadedThumbnails;
            if (ThumbnailTools::LoadThumbnailsFromPackage(FilePath, {AssetName}, LoadedThumbnails)) {
                for (auto it = LoadedThumbnails.begin(); it != LoadedThumbnails.end(); ++it) {
                    UE_LOG(LogAnchorpoint, Log, TEXT("LoadedThumbnails Key: %s"), *it.Key().ToString());
                }
                
                ObjectThumbnail = LoadedThumbnails.Find(AssetName);
                if(!ObjectThumbnail || ObjectThumbnail->GetImageWidth() == 0 || ObjectThumbnail->GetImageHeight() == 0) {
                    ObjectThumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(Asset);
                } else {
                    auto UncompressedImageData = ObjectThumbnail->GetUncompressedImageData();
                    if (!UncompressedImageData.GetData()){
                        // Image could not be uncompressed, re-render
                        ObjectThumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(Asset);
                    }
                }
            } else {
                ObjectThumbnail = ThumbnailTools::GenerateThumbnailForObjectToSaveToDisk(Asset);
            }
        }
        
        if (ObjectThumbnail){
            if (!SaveThumbnailToDisk(ObjectThumbnail, OutputPath))
            {
                UE_LOG(LogAnchorpoint, Error, TEXT("Could not save thumbnail for: %s"), *FilePath);
                return false;
            }
        }
        
        return true;
        
    }
    
    return false;
}
