// Some copyright should be here...

#pragma once

#include <ISourceControlProvider.h>

#include <Misc/EngineVersionComparison.h>

class FAnchorpointSourceControlState;
class FAnchorpointSourceControlCommand;
class IAnchorpointSourceControlWorker;

class FAnchorpointSourceControlProvider : public ISourceControlProvider
{
public:
	FAnchorpointSourceControlProvider();
	virtual ~FAnchorpointSourceControlProvider();

	using FGetAnchorpointSourceControlWorker = TDelegate<TSharedRef<IAnchorpointSourceControlWorker>()>;

	template <typename Type>
	void RegisterWorker(const FName& InName);


	//~ Begin ISourceControlProvider Interface
	virtual void Init(bool bForceConnection) override;
	virtual void Close() override;
	virtual const FName& GetName() const override;
	virtual FText GetStatusText() const override;
#if UE_VERSION_NEWER_THAN(5, 3, 0)
	virtual TMap<EStatus, FString> GetStatus() const override;
#endif
	virtual bool IsEnabled() const override;
	virtual bool IsAvailable() const override;
	virtual bool QueryStateBranchConfig(const FString& ConfigSrc, const FString& ConfigDest) override;
	virtual void RegisterStateBranches(const TArray<FString>& BranchNames, const FString& ContentRoot) override;
	virtual int32 GetStateBranchIndex(const FString& BranchName) const override;
	virtual ECommandResult::Type GetState(const TArray<FString>& InFiles, TArray<FSourceControlStateRef>& OutState, EStateCacheUsage::Type InStateCacheUsage) override;
	virtual ECommandResult::Type GetState(const TArray<FSourceControlChangelistRef>& InChangelists, TArray<FSourceControlChangelistStateRef>& OutState, EStateCacheUsage::Type InStateCacheUsage) override;
	virtual TArray<FSourceControlStateRef> GetCachedStateByPredicate(TFunctionRef<bool(const FSourceControlStateRef&)> Predicate) const override;
	virtual FDelegateHandle RegisterSourceControlStateChanged_Handle(const FSourceControlStateChanged::FDelegate& SourceControlStateChanged) override;
	virtual void UnregisterSourceControlStateChanged_Handle(FDelegateHandle Handle) override;
	virtual ECommandResult::Type Execute(const FSourceControlOperationRef& InOperation, FSourceControlChangelistPtr InChangelist, const TArray<FString>& InFiles, EConcurrency::Type InConcurrency = EConcurrency::Synchronous, const FSourceControlOperationComplete& InOperationCompleteDelegate = FSourceControlOperationComplete()) override;
#if UE_VERSION_NEWER_THAN(5, 3, 0)
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
	virtual bool AllowsDiffAgainstDepot() const override;
	virtual TOptional<bool> IsAtLatestRevision() const override;
	virtual TOptional<int> GetNumLocalChanges() const override;
	virtual void Tick() override;
	virtual TSharedRef<SWidget> MakeSettingsWidget() const override;
	//~ End ISourceControlProvider Interface

	void HandlePackageSaved(const FString& InPackageFilename, UPackage* InPackage, FObjectPostSaveContext InObjectSaveContext);
	void RefreshStatus();

	TSharedRef<FAnchorpointSourceControlState> GetStateInternal(const FString& Filename);
	FDateTime GetLastSyncTime() const;

	TMap<FString, TSharedRef<FAnchorpointSourceControlState>> StateCache;

	void OutputCommandMessages(const FAnchorpointSourceControlCommand& InCommand) const;
	TSharedPtr<IAnchorpointSourceControlWorker> CreateWorker(const FName& OperationName);
	ECommandResult::Type ExecuteSynchronousCommand(FAnchorpointSourceControlCommand& InCommand, const FText& Task);
	ECommandResult::Type IssueCommand(FAnchorpointSourceControlCommand& InCommand);

	TMap<FName, FGetAnchorpointSourceControlWorker> WorkersMap;

	FSourceControlStateChanged OnSourceControlStateChanged;
	TArray<FAnchorpointSourceControlCommand*> CommandQueue;

	FTimerHandle RefreshTimerHandle;
	float RefreshDelay = 1.0f;
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