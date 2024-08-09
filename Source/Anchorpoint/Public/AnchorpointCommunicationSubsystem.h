// Some copyright should be here...

#pragma once

#include "EditorSubsystem.h"

#include "AnchorpointCommunicationSubsystem.generated.h"

#pragma once

UCLASS()
class UAnchorpointCommunicationSubsystem : public UEditorSubsystem, public FTickableGameObject
{
	GENERATED_BODY()
public:
	bool OpenDesktopApp(bool bMinimized = true);

private:
	//~ Begin FTickableGameObject interface
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsAllowedToTick() const override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~ End FTickableGameObject interface

public:
	void UnlinkPackages(const TArray<FString>& PackageNames);
	void RelinkPackages(const TArray<FString>& PackageNames);

	FString IpcSenderId = TEXT("Unreal Engine ") VERSION_STRINGIFY(ENGINE_MAJOR_VERSION) TEXT(".") VERSION_STRINGIFY(ENGINE_MINOR_VERSION) TEXT(".") VERSION_STRINGIFY(ENGINE_PATCH_VERSION);
};