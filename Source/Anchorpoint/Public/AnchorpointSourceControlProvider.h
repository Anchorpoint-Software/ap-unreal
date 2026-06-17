// Copyright (C) 2024-2025 Anchorpoint Software GmbH. All rights reserved.

#pragma once

#include <ISourceControlProvider.h>

#include <Misc/EngineVersionComparison.h>

class FAnchorpointSourceControlState;
class FAnchorpointSourceControlCommand;
class IAnchorpointSourceControlWorker;
struct FAnchorpointStatus;
struct FAnchorpointConnectMessage;

class FAnchorpointSourceControlProvider : public ISourceControlProvider
{
public:
	FAnchorpointSourceControlProvider();
	virtual ~FAnchorpointSourceControlProvider();

	using FGetAnchorpointSourceControlWorker = TDelegate<TSharedRef<IAnchorpointSourceControlWorker>()>;

	template <typename Type>
	void RegisterWorker(const FName& InName);

	bool HasAnyConflicts() const;

	//~ Begin ISourceControlProvider Interface
	virtual void Init(bool bForceConnection) override;
	virtual void Close() override;
	virtual const FName& GetName() const override;
	virtual FText GetStatusText() const override;
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
	virtual TMap<EStatus, FString> GetStatus() const override;
#endif
	virtual bool IsEnabled() const override;
	virtual bool IsAvailable() const override;
	virtual bool QueryStateBranchConfig(const FString& ConfigSrc, const FString& ConfigDest) override;
	virtual void RegisterStateBranches(const TArray<FString>& BranchNames, const FString& ContentRoot) override;
	virtual int32 GetStateBranchIndex(const FString& BranchName) const override;
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 7)
	virtual bool GetStateBranchAtIndex(int32 BranchIndex, FString& OutBranchName) const override;
#endif
	virtual ECommandResult::Type GetState(const TArray<FString>& InFiles, TArray<FSourceControlStateRef>& OutState, EStateCacheUsage::Type InStateCacheUsage) override;
	virtual ECommandResult::Type GetState(const TArray<FSourceControlChangelistRef>& InChangelists, TArray<FSourceControlChangelistStateRef>& OutState, EStateCacheUsage::Type InStateCacheUsage) override;
	virtual TArray<FSourceControlStateRef> GetCachedStateByPredicate(TFunctionRef<bool(const FSourceControlStateRef&)> Predicate) const override;
	virtual FDelegateHandle RegisterSourceControlStateChanged_Handle(const FSourceControlStateChanged::FDelegate& SourceControlStateChanged) override;
	virtual void UnregisterSourceControlStateChanged_Handle(FDelegateHandle Handle) override;
	virtual ECommandResult::Type Execute(const FSourceControlOperationRef& InOperation, FSourceControlChangelistPtr InChangelist, const TArray<FString>& InFiles, EConcurrency::Type InConcurrency = EConcurrency::Synchronous, const FSourceControlOperationComplete& InOperationCompleteDelegate = FSourceControlOperationComplete()) override;
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 3)
	virtual bool CanExecuteOperation(const FSourceControlOperationRef& InOperation) const override;
#endif
	virtual bool CanCancelOperation(const FSourceControlOperationRef& InOperation) const override;
	virtual void CancelOperation(const FSourceControlOperationRef& InOperation) override;
	virtual TArray<TSharedRef<ISourceControlLabel>> GetLabels(const FString& InMatchingSpec) const override;
	virtual TArray<FSourceControlChangelistRef> GetChangelists(EStateCacheUsage::Type InStateCacheUsage) override;
	virtual bool UsesLocalReadOnlyState() const override;
	virtual bool UsesChangelists() const override;
	virtual bool UsesUncontrolledChangelists() const override;
	virtual bool UsesCheckout() const override;
	virtual bool UsesFileRevisions() const override;
	virtual bool UsesSnapshots() const override;
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 8)
	virtual bool UsesSoftRevertOnDelete() const override;
#endif
	virtual bool AllowsDiffAgainstDepot() const override;
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 8)
	virtual TOptional<bool> HasChangesToSync() const override;
	virtual TOptional<bool> HasChangesToCheckIn() const override;
#else
	virtual TOptional<bool> IsAtLatestRevision() const override;
	virtual TOptional<int> GetNumLocalChanges() const override;
#endif
	virtual void Tick() override;
	virtual TSharedRef<SWidget> MakeSettingsWidget() const override;
	//~ End ISourceControlProvider Interface

	/**
	 * Executed when the `Connect` source control command is executed by Anchorpoint
	 */
	void OnConnected();
	void OnStatesChanged();
	void TickDuringModal(float DeltaTime);

	TSharedRef<FAnchorpointSourceControlState> GetStateInternal(const FString& Filename);
	FDateTime GetLastSyncTime() const;

	TMap<FString, TSharedRef<FAnchorpointSourceControlState>> StateCache;

	/**
	 * Tries to patch the cached status when an asset is saved.
	 */
	bool OnAssetSavedPatchStatus(FAnchorpointStatus& InOutStatus, const FString& InPackageFilename);
	/**
	 * Tries to patch the cached status when a message is received.
	 */
	bool OnMessageReceivedPatchStatus(FAnchorpointStatus& InOutStatus, const FAnchorpointConnectMessage& InMessage);
	/**
	 * Tries to patch the cached status when an asset is created in memory (e.g. via duplication).
	 */
	bool OnInMemoryAssetCreatedPatchStatus(FAnchorpointStatus& InOutStatus, UObject* InAsset);

	void OutputCommandMessages(const FAnchorpointSourceControlCommand& InCommand) const;
	TSharedPtr<IAnchorpointSourceControlWorker> CreateWorker(const FName& OperationName);
	ECommandResult::Type ExecuteSynchronousCommand(FAnchorpointSourceControlCommand& InCommand, const FText& TaskPrompt);
	ECommandResult::Type IssueCommand(FAnchorpointSourceControlCommand& InCommand);

	TMap<FName, FGetAnchorpointSourceControlWorker> WorkersMap;

	FSourceControlStateChanged OnSourceControlStateChanged;
	TArray<FAnchorpointSourceControlCommand*> CommandQueue;

	FText GetPromptTextForOperation(const FSourceControlOperationRef& InOperation) const;

	enum class EActiveModalState
	{
		None,
		Submit,
		PackagesDialog, // also known as "Check out Assets" dialog
		WorldPartitionBuildNavigation,
	} ActiveModalState = EActiveModalState::None;
};

template <typename Type>
void FAnchorpointSourceControlProvider::RegisterWorker(const FName& InName)
{
	FGetAnchorpointSourceControlWorker Delegate = FGetAnchorpointSourceControlWorker::CreateLambda([]()
	{
		return MakeShared<Type>();
	});

	WorkersMap.Add(InName, Delegate);
}