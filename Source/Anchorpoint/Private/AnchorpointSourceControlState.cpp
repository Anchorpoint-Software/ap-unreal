// Some copyright should be here...

#include "AnchorpointSourceControlState.h"

#include <RevisionControlStyle/RevisionControlStyle.h>

#define LOCTEXT_NAMESPACE "Anchorpoint"

FAnchorpointSourceControlState::FAnchorpointSourceControlState(const FString& InLocalFilename)
{
	LocalFilename = InLocalFilename;
}

int32 FAnchorpointSourceControlState::GetHistorySize() const
{
	return 0;
}

TSharedPtr<ISourceControlRevision> FAnchorpointSourceControlState::GetHistoryItem(int32 HistoryIndex) const
{
	return nullptr;
}

TSharedPtr<ISourceControlRevision> FAnchorpointSourceControlState::FindHistoryRevision(int32 RevisionNumber) const
{
	return nullptr;
}

TSharedPtr<ISourceControlRevision> FAnchorpointSourceControlState::FindHistoryRevision(const FString& InRevision) const
{
	return nullptr;
}

TSharedPtr<ISourceControlRevision> FAnchorpointSourceControlState::GetCurrentRevision() const
{
	return nullptr;
}

#if UE_VERSION_OLDER_THAN(5, 3, 0)
TSharedPtr<ISourceControlRevision> FAnchorpointSourceControlState::GetBaseRevForMerge() const
{
	return nullptr;
}
#endif

#if UE_VERSION_NEWER_THAN(5, 3, 0)

ISourceControlState::FResolveInfo FAnchorpointSourceControlState::GetResolveInfo() const
{
	return PendingResolveInfo;
}

#endif

#if SOURCE_CONTROL_WITH_SLATE

FSlateIcon FAnchorpointSourceControlState::GetIcon() const
{
	switch (State)
	{
	case EAnchorpointState::Unknown:
		return FSlateIcon();
	case EAnchorpointState::Added:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.OpenForAdd");
	case EAnchorpointState::Ignored:
		return FSlateIcon();
	case EAnchorpointState::OutDated:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.NotAtHeadRevision");
	case EAnchorpointState::LockedBySomeone:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOutByOtherUser", NAME_None, "RevisionControl.CheckedOutByOtherUserBadge");
	case EAnchorpointState::LockedModified:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOut");
	case EAnchorpointState::LockedDeleted:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.MarkedForDelete");
	case EAnchorpointState::LockedUnchanged:
		return FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Lock");
	case EAnchorpointState::UnlockedModified:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.ModifiedLocally");
	case EAnchorpointState::UnlockedDeleted:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.ModifiedLocally");
	case EAnchorpointState::UnlockedUnchanged:
		return FSlateIcon();
	case EAnchorpointState::Conflicted:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Conflicted");

	default:
		checkNoEntry();
	}

	return FSlateIcon();
}

#endif // SOURCE_CONTROL_WITH_SLATE

FText FAnchorpointSourceControlState::GetDisplayName() const
{
	switch (State)
	{
	case EAnchorpointState::Unknown:
		return LOCTEXT("Unknown", "Unknown");
	case EAnchorpointState::Added:
		return LOCTEXT("Added", "Added");
	case EAnchorpointState::Ignored:
		return LOCTEXT("Ignored", "Ignored");
	case EAnchorpointState::OutDated:
		return LOCTEXT("OutDated", "Outdated");
	case EAnchorpointState::LockedBySomeone:
		return FText::Format(LOCTEXT("CheckedOutOther", "Locked by: {0}"), FText::FromString(OtherUserCheckedOut));
	case EAnchorpointState::LockedModified:
		return LOCTEXT("LockedModified", "Modified (locked)");
	case EAnchorpointState::LockedDeleted:
		return LOCTEXT("LockedDeleted", "Deleted (locked)");
	case EAnchorpointState::LockedUnchanged:
		return LOCTEXT("LockedUnchanged", "Unchanged (locked)");
	case EAnchorpointState::UnlockedModified:
		return LOCTEXT("UnlockedModified", "Modified (not locked)");
	case EAnchorpointState::UnlockedDeleted:
		return LOCTEXT("UnlockedDeleted", "Deleted (not locked)");
	case EAnchorpointState::UnlockedUnchanged:
		return LOCTEXT("UnlockedUnchanged", "Unchanged (not locked)");
	case EAnchorpointState::Conflicted:
		return LOCTEXT("ContentsConflict", "Contents Conflict");

	default:
		checkNoEntry();
	}

	return FText();
}

FText FAnchorpointSourceControlState::GetDisplayTooltip() const
{
	switch (State)
	{
	case EAnchorpointState::Unknown:
		return LOCTEXT("Unknown_Tooltip", "Unknown");
	case EAnchorpointState::Ignored:
		return LOCTEXT("Ignored_Tooltip", "Ignored");
	case EAnchorpointState::Added:
		return LOCTEXT("Added_Tooltip", "Added");
	case EAnchorpointState::OutDated:
		return LOCTEXT("OutDated_Tooltip", "Outdated");
	case EAnchorpointState::LockedBySomeone:
		return FText::Format(LOCTEXT("CheckedOutOther_Tooltip", "Locked by: {0}"), FText::FromString(OtherUserCheckedOut));
	case EAnchorpointState::LockedModified:
		return LOCTEXT("LockedModified_Tooltip", "Modified (locked)");
	case EAnchorpointState::LockedDeleted:
		return LOCTEXT("LockedDeleted_Tooltip", "Deleted (locked)");
	case EAnchorpointState::LockedUnchanged:
        return LOCTEXT("LockedUnchanged_Tooltip", "Unchanged (locked)");
	case EAnchorpointState::UnlockedModified:
		return LOCTEXT("UnlockedModified_Tooltip", "Modified (not locked)");
	case EAnchorpointState::UnlockedDeleted:
		return LOCTEXT("UnlockedDeleted_Tooltip", "Deleted (not locked)");
	case EAnchorpointState::UnlockedUnchanged:
		return LOCTEXT("UnlockedUnchanged_Tooltip", "Unchanged (not locked)");
	case EAnchorpointState::Conflicted:
		return LOCTEXT("ContentsConflict_Tooltip", "Contents Conflict");

	default:
		checkNoEntry();
	}

	return FText();
}

TOptional<FText> FAnchorpointSourceControlState::GetStatusText() const
{
	// Same as ISourceControlState::GetStatusText() but we display it even if the asset is not CheckedOut
	const bool bWorthDisplaying = State != EAnchorpointState::UnlockedUnchanged;
	
	TOptional<FText> StatusText = GetWarningText();
	if (!StatusText.IsSet() && bWorthDisplaying)
	{
		StatusText.Emplace(GetDisplayTooltip());
	}
	return StatusText;
}

const FString& FAnchorpointSourceControlState::GetFilename() const
{
	return LocalFilename;
}

const FDateTime& FAnchorpointSourceControlState::GetTimeStamp() const
{
	return TimeStamp;
}

bool FAnchorpointSourceControlState::CanCheckIn() const
{
	return State == EAnchorpointState::Added
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted;
}

bool FAnchorpointSourceControlState::CanCheckout() const
{
	return State == EAnchorpointState::UnlockedUnchanged
		|| State == EAnchorpointState::UnlockedModified
		|| State == EAnchorpointState::UnlockedDeleted;
}

bool FAnchorpointSourceControlState::IsCheckedOut() const
{
	return State == EAnchorpointState::Added
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted
		|| State == EAnchorpointState::LockedUnchanged;
}

bool FAnchorpointSourceControlState::IsCheckedOutOther(FString* Who) const
{
	if (Who != nullptr)
	{
		*Who = OtherUserCheckedOut;
	}

	return State == EAnchorpointState::LockedBySomeone;
}

bool FAnchorpointSourceControlState::IsCheckedOutInOtherBranch(const FString& CurrentBranch /* = FString() */) const
{
	return false;
}

bool FAnchorpointSourceControlState::IsModifiedInOtherBranch(const FString& CurrentBranch /* = FString() */) const
{
	return false;
}

bool FAnchorpointSourceControlState::GetOtherBranchHeadModification(FString& HeadBranchOut, FString& ActionOut, int32& HeadChangeListOut) const
{
	return false;
}

bool FAnchorpointSourceControlState::IsCurrent() const
{
	// TODO: Hack to disable the `Sync` option
	return true;
	// return State != EAnchorpointState::OutDated;
}

bool FAnchorpointSourceControlState::IsSourceControlled() const
{
	return State != EAnchorpointState::Unknown;
}

bool FAnchorpointSourceControlState::IsAdded() const
{
	return State == EAnchorpointState::Added;
}

bool FAnchorpointSourceControlState::IsDeleted() const
{
	return State == EAnchorpointState::LockedDeleted;
}

bool FAnchorpointSourceControlState::IsIgnored() const
{
	return State == EAnchorpointState::Ignored;
}

bool FAnchorpointSourceControlState::CanEdit() const
{
	//TODO: Confirm with AP team if we want to prevent locked files from being modified
	return true; // With Git all files in the working copy are always editable (as opposed to Perforce)
}

bool FAnchorpointSourceControlState::CanDelete() const
{
	return IsSourceControlled() && IsCurrent();
}

bool FAnchorpointSourceControlState::IsUnknown() const
{
	return State == EAnchorpointState::Unknown;
}

bool FAnchorpointSourceControlState::IsModified() const
{
	return State == EAnchorpointState::Added
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted
		|| State == EAnchorpointState::UnlockedModified
		|| State == EAnchorpointState::UnlockedDeleted;
}

bool FAnchorpointSourceControlState::CanAdd() const
{
	// Files are automatically added to the repo when they are created, the user can not manually add them
	return false;
}

bool FAnchorpointSourceControlState::IsConflicted() const
{
	return State == EAnchorpointState::Conflicted;
}

bool FAnchorpointSourceControlState::CanRevert() const
{
	//TODO: Test if reverting an added file just deletes it
	return State == EAnchorpointState::Added
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted
		|| State == EAnchorpointState::LockedUnchanged
		|| State == EAnchorpointState::UnlockedModified
		|| State == EAnchorpointState::UnlockedDeleted;
}

#undef LOCTEXT_NAMESPACE