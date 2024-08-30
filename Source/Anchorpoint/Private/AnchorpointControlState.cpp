﻿// Some copyright should be here...

#include "AnchorpointControlState.h"

#include <RevisionControlStyle/RevisionControlStyle.h>

#define LOCTEXT_NAMESPACE "Anchorpoint"

FAnchorpointControlState::FAnchorpointControlState(const FString& InLocalFilename)
{
	LocalFilename = InLocalFilename;
}

int32 FAnchorpointControlState::GetHistorySize() const
{
	return 0;
}

TSharedPtr<ISourceControlRevision> FAnchorpointControlState::GetHistoryItem(int32 HistoryIndex) const
{
	return nullptr;
}

TSharedPtr<ISourceControlRevision> FAnchorpointControlState::FindHistoryRevision(int32 RevisionNumber) const
{
	return nullptr;
}

TSharedPtr<ISourceControlRevision> FAnchorpointControlState::FindHistoryRevision(const FString& InRevision) const
{
	return nullptr;
}

TSharedPtr<ISourceControlRevision> FAnchorpointControlState::GetCurrentRevision() const
{
	return nullptr;
}

ISourceControlState::FResolveInfo FAnchorpointControlState::GetResolveInfo() const
{
	return PendingResolveInfo;
}

#if SOURCE_CONTROL_WITH_SLATE

FSlateIcon FAnchorpointControlState::GetIcon() const
{
	switch (State)
	{
	case EAnchorpointState::Unknown:
		return FSlateIcon();
	case EAnchorpointState::Ignored:
		return FSlateIcon();
	case EAnchorpointState::OutDated:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.NotAtHeadRevision");
	case EAnchorpointState::LockedBySomeone:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOutByOtherUser", NAME_None, "RevisionControl.CheckedOutByOtherUserBadge");
	case EAnchorpointState::LockedAdded:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.OpenForAdd");
	case EAnchorpointState::LockedModified:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOut");
	case EAnchorpointState::LockedDeleted:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.MarkedForDelete");
	case EAnchorpointState::LockedUnchanged:
		return FSlateIcon(FAppStyle::GetAppStyleSetName(), "Icons.Lock");;
	case EAnchorpointState::UnlockedAdded:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.ModifiedLocally");
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

FText FAnchorpointControlState::GetDisplayName() const
{
	switch (State)
	{
	case EAnchorpointState::Unknown:
		return LOCTEXT("Unknown", "Unknown");
	case EAnchorpointState::Ignored:
		return LOCTEXT("Ignored", "Ignored");
	case EAnchorpointState::OutDated:
		return LOCTEXT("NotCurrent", "Not current");
	case EAnchorpointState::LockedBySomeone:
		return FText::Format(LOCTEXT("CheckedOutOther", "Locked out by: {0}"), FText::FromString(OtherUserCheckedOut));
	case EAnchorpointState::LockedAdded:
		return LOCTEXT("LockedAdded", "Added (locked)");
	case EAnchorpointState::LockedModified:
		return LOCTEXT("LockedModified", "Modified (locked)");
	case EAnchorpointState::LockedDeleted:
		return LOCTEXT("LockedDeleted", "Deleted (locked)");
	case EAnchorpointState::LockedUnchanged:
		return LOCTEXT("LockedUnchanged", "Unchanged (locked)");
	case EAnchorpointState::UnlockedAdded:
		return LOCTEXT("UnlockedAdded", "Added (not locked)");
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

FText FAnchorpointControlState::GetDisplayTooltip() const
{
	switch (State)
	{
	case EAnchorpointState::Unknown:
		return LOCTEXT("Unknown_Tooltip", "Unknown");
	case EAnchorpointState::Ignored:
		return LOCTEXT("Ignored_Tooltip", "Ignored");
	case EAnchorpointState::OutDated:
		return LOCTEXT("NotCurrent_Tooltip", "Not current");
	case EAnchorpointState::LockedBySomeone:
		return FText::Format(LOCTEXT("CheckedOutOther_Tooltip", "Locked out by: {0}"), FText::FromString(OtherUserCheckedOut));
	case EAnchorpointState::LockedAdded:
		return LOCTEXT("LockedAdded_Tooltip", "Added (locked)");
	case EAnchorpointState::LockedModified:
		return LOCTEXT("LockedModified_Tooltip", "Modified (locked)");
	case EAnchorpointState::LockedDeleted:
		return LOCTEXT("LockedDeleted_Tooltip", "Deleted (locked)");
	case EAnchorpointState::LockedUnchanged:
        return LOCTEXT("LockedUnchanged_Tooltip", "Unchanged (locked)");
	case EAnchorpointState::UnlockedAdded:
		return LOCTEXT("UnlockedAdded_Tooltip", "Added (not locked)");
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

const FString& FAnchorpointControlState::GetFilename() const
{
	return LocalFilename;
}

const FDateTime& FAnchorpointControlState::GetTimeStamp() const
{
	return TimeStamp;
}

bool FAnchorpointControlState::CanCheckIn() const
{
	return State == EAnchorpointState::LockedAdded
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted;
}

bool FAnchorpointControlState::CanCheckout() const
{
	return State == EAnchorpointState::UnlockedUnchanged;
}

bool FAnchorpointControlState::IsCheckedOut() const
{
	return State == EAnchorpointState::LockedAdded
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted
		|| State == EAnchorpointState::LockedUnchanged;
}

bool FAnchorpointControlState::IsCheckedOutOther(FString* Who) const
{
	if (Who != nullptr)
	{
		*Who = OtherUserCheckedOut;
	}

	return State == EAnchorpointState::LockedBySomeone;
}

bool FAnchorpointControlState::IsCheckedOutInOtherBranch(const FString& CurrentBranch /* = FString() */) const
{
	return false;
}

bool FAnchorpointControlState::IsModifiedInOtherBranch(const FString& CurrentBranch /* = FString() */) const
{
	return false;
}

bool FAnchorpointControlState::GetOtherBranchHeadModification(FString& HeadBranchOut, FString& ActionOut, int32& HeadChangeListOut) const
{
	return false;
}

bool FAnchorpointControlState::IsCurrent() const
{
	return State != EAnchorpointState::OutDated;
}

bool FAnchorpointControlState::IsSourceControlled() const
{
	return State != EAnchorpointState::Unknown;
}

bool FAnchorpointControlState::IsAdded() const
{
	return State == EAnchorpointState::LockedAdded;
}

bool FAnchorpointControlState::IsDeleted() const
{
	return State == EAnchorpointState::LockedDeleted;
}

bool FAnchorpointControlState::IsIgnored() const
{
	return State == EAnchorpointState::Ignored;
}

bool FAnchorpointControlState::CanEdit() const
{
	//TODO: Confirm with AP team if we want to prevent locked files from being modified
	return true; // With Git all files in the working copy are always editable (as opposed to Perforce)
}

bool FAnchorpointControlState::CanDelete() const
{
	return IsSourceControlled() && IsCurrent();
}

bool FAnchorpointControlState::IsUnknown() const
{
	return State == EAnchorpointState::Unknown;
}

bool FAnchorpointControlState::IsModified() const
{
	return State == EAnchorpointState::LockedAdded
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted
		|| State == EAnchorpointState::UnlockedAdded
		|| State == EAnchorpointState::UnlockedModified
		|| State == EAnchorpointState::UnlockedDeleted;
}

bool FAnchorpointControlState::CanAdd() const
{
	//TODO: Confirm with AP team if a files is created on disk it is automatically added and the user doesn't have to do it manually ever
	return false; // Files are automatically added by AP 
}

bool FAnchorpointControlState::IsConflicted() const
{
	return State == EAnchorpointState::Conflicted;
}

bool FAnchorpointControlState::CanRevert() const
{
	return State == EAnchorpointState::LockedAdded
		|| State == EAnchorpointState::LockedModified
		|| State == EAnchorpointState::LockedDeleted
		|| State == EAnchorpointState::LockedUnchanged
		|| State == EAnchorpointState::UnlockedAdded
		|| State == EAnchorpointState::UnlockedModified
		|| State == EAnchorpointState::UnlockedDeleted;
}

#undef LOCTEXT_NAMESPACE