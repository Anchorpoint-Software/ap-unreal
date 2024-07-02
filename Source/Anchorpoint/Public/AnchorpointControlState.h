// Some copyright should be here...

#pragma once

#include <ISourceControlState.h>

/**
 * 
 */
class FAnchorpointControlState : public ISourceControlState
{
public:
	explicit FAnchorpointControlState(FString&& InLocalFilename)
		: LocalFilename(MoveTemp(InLocalFilename))
	{
	}

	FAnchorpointControlState() = delete;
	FAnchorpointControlState(const FAnchorpointControlState& InState) = delete;
	const FAnchorpointControlState& operator=(const FAnchorpointControlState& InState) = delete;

	// debug log utility
	const TCHAR* ToString() const;

	/** ISourceControlState interface */
	virtual int32 GetHistorySize() const override;
	virtual TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> GetHistoryItem(int32 HistoryIndex) const override;
	virtual TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> FindHistoryRevision(int32 RevisionNumber) const override;
	virtual TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> FindHistoryRevision(const FString& InRevision) const override;
	virtual TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> GetCurrentRevision() const override;
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
	virtual bool IsCheckedOutOther(FString* Who = nullptr) const override;
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

public:
	/** Filename on disk */
	FString LocalFilename;

	/** Depot and Server info (in the form repo@server:port) */
	FString RepSpec;

	/** Pending rev info with which a file must be resolved, invalid if no resolve pending */
	FResolveInfo PendingResolveInfo;

	/** Unity Version Control Parameters of the merge in progress */
	TArray<FString> PendingMergeParameters;

	/** If a user (another or ourself) has this file locked, this contains their name. */
	FString LockedBy;

	/** Location (Workspace) where the file was exclusively checked-out. */
	FString LockedWhere;

	/** Branch where the file was Locked or is Retained. */
	FString LockedBranch;

	/** Item id of the locked file (for an admin to unlock it). */
	int32 LockedId = INVALID_REVISION;

	/** Date when the file was Locked. */
	FDateTime LockedDate = 0;

	/** If a user (another or ourself) has this file Retained on another branch, this contains their name. */
	FString RetainedBy;

	/** Latest revision number of the file in the depot (on the current branch) */
	int32 DepotRevisionChangeset = INVALID_REVISION;

	/** Latest revision number at which a file was synced to before being edited */
	int32 LocalRevisionChangeset = INVALID_REVISION;

	/** Original name in case of a Moved/Renamed file */
	FString MovedFrom;

	/** The timestamp of the last update */
	FDateTime TimeStamp = 0;

	/** The branch with the head change list */
	FString HeadBranch;

	/** The type of action of the last modification */
	FString HeadAction;

	/** The user of the last modification */
	FString HeadUserName;

	/** The last file modification time */
	int64 HeadModTime;

	/** The change list of the last modification */
	int32 HeadChangeList;
};