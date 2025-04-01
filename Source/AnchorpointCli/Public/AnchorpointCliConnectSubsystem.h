// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <EditorSubsystem.h>

#include "AnchorpointCliStatus.h"

#include "AnchorpointCliConnectSubsystem.generated.h"

class FAnchorpointCliProcess;
/**
 * Structure of the JSON messages received from the Anchorpoint CLI connect
 */
USTRUCT()
struct FAnchorpointConnectMessage 
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Type;

	UPROPERTY()
	TArray<FString> Files;

	static TOptional<TArray<FAnchorpointConnectMessage>> ParseStringToMessages(const FString& InString);
};

/**
 * Structure of the JSON messages sent to the Anchorpoint CLI connect
 */
USTRUCT()
struct FAnchorpointCliConnectResponse 
{
	GENERATED_BODY()

	UPROPERTY()
	FString Id;

	UPROPERTY()
	FString Error;
};

/**
 * Subsystem responsible for handling the live connection to the Anchorpoint CLI
 */
UCLASS()
class ANCHORPOINTCLI_API UAnchorpointCliConnectSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	/**
	 * Returns the current cached version of the status if caching is enabled
	 */
	TOptional<FAnchorpointStatus> GetCachedStatus() const;
	/**
	 * Updates the cached version of the status with a newer value if caching is enabled
	 */
	void UpdateStatusCacheIfPossible(const FAnchorpointStatus& Status);
	/**
	 * Callback executed after a message is received and parsed, but before it is handled
	 */
	TMulticastDelegate<void(const FAnchorpointConnectMessage& InMessage)> OnPreMessageHandled;
	/**
	 * Checks if the integration is currently connected to the Anchorpoint CLI
	 */
	bool IsConnected() const;

private:
	//~ Begin UEditorSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~ End UEditorSubsystem Interface

	/**
	 * Called at fixed intervals to check the status of the connection
	 */
	bool Tick(const float InDeltaTime);
	/**
	 * Checks if the connection to the Anchorpoint CLI is established and if not, starts the connection
	 */
	void TickConnection();
	/**
	 * Refreshes the Source Control status of the files in the message 
	 */
	void RefreshStatus(const FAnchorpointConnectMessage& Message);
	/**
	 * Checks if the project has been saved and if not, returns an error message
	 */
	TOptional<FString> CheckProjectSaveStatus(const TArray<FString>& Files);
	/**
	 * Stats the sync process by unlinking the files in the message
	 */
	void StartSync(const FAnchorpointConnectMessage& Message);
	/**
	 * Unlinks and relinks the packages in order to allow Anchorpoint to perform the pull externally
	 */
	void PerformSync(const FAnchorpointConnectMessage& Message);
	/**
	 * Completes the sync process by reloading the files in the message
	 */
	void StopSync(const FAnchorpointConnectMessage& Message);

	/**
	 * Callback executed when the Anchorpoint Source Control Provider is initialized
	 */
	void OnAnchorpointProviderConnected();
	/**
	 * Callback executed when a message is received from the Anchorpoint CLI
	 */
	void HandleMessage(const FAnchorpointConnectMessage& Message);
	/**
	 * Function used to reply to a certain message based their Id via stdin. Optionally send an error message.
	 */
	void RespondToMessage(const FString& Id, TOptional<FString> Error = TOptional<FString>());
	/**
	 * Callback executed when the CLI process is updated 
	 */
	void OnProcessUpdated();
	/**
	 * Callback executed when the CLI process finishes 
	 */
	void OnProcessEnded();
	/**
	 * Callback executed to determine if the sync operation is completed
	 */
	bool UpdateSync(const TArray<FString>& PackageFilenames);
	/**
	 * Callback executed to handle the fake Anchorpoint CLI messages sent via console commands
	 */
	void OnFakeAnchorpointCliMessage(const TArray<FString>& Params, UWorld* InWorld, FOutputDevice& Ar);
	/**
	 * Displays a toast message indicating the connection state
	 */
	void ToastConnectionState(bool bConnected);
	/**
	 * The process that is running the Anchorpoint CLI connect command
	 */
	TSharedPtr<FAnchorpointCliProcess> Process = nullptr;
	/**
	 * Sanity check to ensure that the sync operation is not started multiple times
	 */
	bool bSyncInProgress = false;
	/**
	 * Indicates if the project is connected, and we should use the status cache to avoid unnecessary status checks
	 */
	bool bCanUseStatusCache = false;
	/**
	 * Result of the last "status" command. This should contain all the files in the project not just the ones in the message
	 */
	TOptional<FAnchorpointStatus> StatusCache;
	/**
	 * Command line console object used to handle the fake Anchorpoint CLI messages
	 */
	IConsoleObject* FakeAnchorpointCliMessage = nullptr;
};

