// Copyright Anchorpoint Software GmbH. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "apsync/ipc/ipc_api.h"
#include "apsync/color/color_api.h"
#include "apsync/projects/projects_api.h"
#include <memory>
#include "Runtime/Core/Public/Misc/Build.h"

#include "Subscription.generated.h"


ANCHORPOINT_API DECLARE_LOG_CATEGORY_EXTERN(LogAnchorpoint, Log, All);

const std::string IpcSenderId = "Unreal Engine " + std::to_string(ENGINE_MAJOR_VERSION) + "." + std::to_string(ENGINE_MINOR_VERSION) + "." + std::to_string(ENGINE_PATCH_VERSION);

/**
 * 
 */
UCLASS()
class ANCHORPOINT_API USubscription : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
	
public:
    virtual ~USubscription();
    void Init(std::shared_ptr<apsync::Api> Api);
    bool IsTickableInEditor() const override;
    TStatId GetStatId() const override;
    void Tick(float DeltaTime) override;
    
private:
    void PublishMessage(apsync::IpcMessage& Message);
    void HandleMessage(const apsync::IpcMessage& Message);
    std::optional<apsync::IpcMessage> HandleThumbnailRequest(const apsync::IpcMessage& Message);
    
    bool SaveThumbnail(FString ObjectPath, FString OutputPath);
    
    std::shared_ptr<apsync::Api> _Api = nullptr;
    std::shared_ptr<apsync::IpcApi> _IpcApi = nullptr;
    std::shared_ptr<apsync::ColorApi> _ColorApi = nullptr;
    std::shared_ptr<apsync::ProjectsApi> _ProjectsApi = nullptr;
    
    apsync::IpcSubscription _IpcSubscription;
    
    FTimerHandle _TimerHandle;
    float _ElapsedTime = 0.0f;
    
    const std::string _IpcTopic = "AnchorpointIPC";
};
