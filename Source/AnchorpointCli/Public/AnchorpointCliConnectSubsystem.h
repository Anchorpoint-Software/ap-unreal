#pragma once

#include <EditorSubsystem.h>

#include "AnchorpointCliStatus.h"

#include "AnchorpointCliConnectSubsystem.generated.h"

class FInteractiveProcess;

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

private:
	//~ Begin UEditorSubsystem Interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
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
	 * TODO: In the future this should check only specific files, now it checks the whole project
	 */
	void CheckProjectSaveStatus(const FAnchorpointConnectMessage& Message);
	/**
	 * Stats the sync process by unlinking the files in the message
	 */
	void StartSync(const FAnchorpointConnectMessage& Message);
	/**
	 * Completes the sync process by reloading the files in the message
	 */
	void StopSync(const FAnchorpointConnectMessage& Message);

	/**
	 * Callback executed when a message is received from the Anchorpoint CLI
	 */
	void HandleMessage(const FAnchorpointConnectMessage& Message);
	/**
	 * Function used to reply to a certain message based their Id via stdin. Optionally send an error message.
	 */
	void RespondToMessage(const FString& Id, TOptional<FString> Error = TOptional<FString>());
	/**
	 * Callback executed when the CLI produces any output
	 */
	void OnOutput(const FString& Output);
	/**
	 * Callback executed when the CLI process completes
	 */
	void OnCompleted(int ReturnCode, bool bCanceling);
	/**
	 * Callback executed when the CLI process is canceled
	 */
	void OnCanceled();
	/**
	 * Callback executed to determine if the sync operation is completed
	 */
	bool UpdateSync(const TArray<FString>& PackageFilenames);
	/**
	 * The process that is running the Anchorpoint CLI connect command
	 */
	TSharedPtr<FInteractiveProcess> Process = nullptr;
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
};

