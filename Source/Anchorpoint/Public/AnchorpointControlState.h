// Some copyright should be here...

#pragma once

#include <ISourceControlState.h>

enum class EAnchorpointState
{
	Unknown,
	LockedBySomeone,
	LockedAdded,
	LockedDeleted,
	LockedModified,
	LockedUnchanged,
	UnlockedAdded,
	UnlockedDeleted,
	UnlockedModified,
	UnlockedUnchanged,

	//TODO: Icons
	OutDated, // if you modify this file, you will cause a conflict
	Conflicted, // you already caused a conflict

	// TODO: Do later
	Ignored,
};

/**
 * 
 */
class FAnchorpointControlState : public ISourceControlState
{
public:
	explicit FAnchorpointControlState(const FString& InLocalFilename);

	FAnchorpointControlState() = delete;
	FAnchorpointControlState(const FAnchorpointControlState& Other) = default;
	FAnchorpointControlState(FAnchorpointControlState&& Other) noexcept = default;
	FAnchorpointControlState& operator=(const FAnchorpointControlState& Other) = default;
	FAnchorpointControlState& operator=(FAnchorpointControlState&& Other) noexcept = default;

	FString LocalFilename;
	FDateTime TimeStamp = 0;
	FString OtherUserCheckedOut;
	FResolveInfo PendingResolveInfo;
	EAnchorpointState State = EAnchorpointState::Unknown;

private:
	/** Being ISourceControlState interface */
	virtual int32 GetHistorySize() const override;
	virtual TSharedPtr<ISourceControlRevision> GetHistoryItem(int32 HistoryIndex) const override;
	virtual TSharedPtr<ISourceControlRevision> FindHistoryRevision(int32 RevisionNumber) const override;
	virtual TSharedPtr<ISourceControlRevision> FindHistoryRevision(const FString& InRevision) const override;
	virtual TSharedPtr<ISourceControlRevision> GetCurrentRevision() const override;
	virtual FResolveInfo GetResolveInfo() const override;
#if SOURCE_CONTROL_WITH_SLATE
	virtual FSlateIcon GetIcon() const override;
#endif // SOURCE_CONTROL_WITH_SLATE
	virtual FText GetDisplayName() const override;
	virtual FText GetDisplayTooltip() const override;
	virtual const FString& GetFilename() const override;
	virtual const FDateTime& GetTimeStamp() const override;
	virtual bool CanCheckIn() const override;
	virtual bool CanCheckout() const override;
	virtual bool IsCheckedOut() const override;
	virtual bool IsCheckedOutOther(FString* Who) const override;
	virtual bool IsCheckedOutInOtherBranch(const FString& CurrentBranch = FString()) const override;
	virtual bool IsModifiedInOtherBranch(const FString& CurrentBranch = FString()) const override;
	virtual bool IsCheckedOutOrModifiedInOtherBranch(const FString& CurrentBranch = FString()) const override { return IsCheckedOutInOtherBranch(CurrentBranch) || IsModifiedInOtherBranch(CurrentBranch); }
	virtual TArray<FString> GetCheckedOutBranches() const override { return TArray<FString>(); }
	virtual FString GetOtherUserBranchCheckedOuts() const override { return FString(); }
	virtual bool GetOtherBranchHeadModification(FString& HeadBranchOut, FString& ActionOut, int32& HeadChangeListOut) const override;
	virtual bool IsCurrent() const override;
	virtual bool IsSourceControlled() const override;
	virtual bool IsAdded() const override;
	virtual bool IsDeleted() const override;
	virtual bool IsIgnored() const override;
	virtual bool CanEdit() const override;
	virtual bool IsUnknown() const override;
	virtual bool IsModified() const override;
	virtual bool CanAdd() const override;
	virtual bool CanDelete() const override;
	virtual bool IsConflicted() const override;
	virtual bool CanRevert() const override;
	/** End ISourceControlState interface */
};