// Some copyright should be here...

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
	case EAnchorpointState::Modified:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOut");
	case EAnchorpointState::Added:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.OpenForAdd");
	case EAnchorpointState::Renamed:
	case EAnchorpointState::Copied:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Branched");
	case EAnchorpointState::Deleted: // Deleted & Missing files does not show in Content Browser
	case EAnchorpointState::Missing:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.MarkedForDelete");
	case EAnchorpointState::Conflicted:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.Conflicted");
	case EAnchorpointState::NotControlled:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.NotInDepot");
	case EAnchorpointState::OutDated:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.NotAtHeadRevision");
	case EAnchorpointState::Locked:
		return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.CheckedOutByOtherUser", NAME_None, "RevisionControl.CheckedOutByOtherUserBadge");

	case EAnchorpointState::Unknown:
	case EAnchorpointState::Unchanged: // Unchanged is the same as "Pristine" (not checked out) for Perforce, ie no icon
	case EAnchorpointState::Ignored:
		return FSlateIcon();
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
	case EAnchorpointState::Unchanged:
		return LOCTEXT("Unchanged", "Unchanged");
	case EAnchorpointState::Added:
		return LOCTEXT("Added", "Added");
	case EAnchorpointState::Deleted:
		return LOCTEXT("Deleted", "Deleted");
	case EAnchorpointState::Modified:
		return LOCTEXT("Modified", "Modified");
	case EAnchorpointState::Renamed:
		return LOCTEXT("Renamed", "Renamed");
	case EAnchorpointState::Copied:
		return LOCTEXT("Copied", "Copied");
	case EAnchorpointState::Conflicted:
		return LOCTEXT("ContentsConflict", "Contents Conflict");
	case EAnchorpointState::Ignored:
		return LOCTEXT("Ignored", "Ignored");
	case EAnchorpointState::NotControlled:
		return LOCTEXT("NotControlled", "Not Under Revision Control");
	case EAnchorpointState::Missing:
		return LOCTEXT("Missing", "Missing");
	case EAnchorpointState::OutDated:
		return LOCTEXT("NotCurrent", "Not current");
	case EAnchorpointState::Locked:
		return FText::Format(LOCTEXT("CheckedOutOther", "Locked out by: {0}"), FText::FromString(OtherUserCheckedOut));
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
		return LOCTEXT("Unknown_Tooltip", "Unknown revision control state");
	case EAnchorpointState::Unchanged:
		return LOCTEXT("Pristine_Tooltip", "There are no modifications");
	case EAnchorpointState::Added:
		return LOCTEXT("Added_Tooltip", "Item is scheduled for addition");
	case EAnchorpointState::Deleted:
		return LOCTEXT("Deleted_Tooltip", "Item is scheduled for deletion");
	case EAnchorpointState::Modified:
		return LOCTEXT("Modified_Tooltip", "Item has been modified");
	case EAnchorpointState::Renamed:
		return LOCTEXT("Renamed_Tooltip", "Item has been renamed");
	case EAnchorpointState::Copied:
		return LOCTEXT("Copied_Tooltip", "Item has been copied");
	case EAnchorpointState::Conflicted:
		return LOCTEXT("ContentsConflict_Tooltip", "The contents of the item conflict with updates received from the repository.");
	case EAnchorpointState::Ignored:
		return LOCTEXT("Ignored_Tooltip", "Item is being ignored.");
	case EAnchorpointState::NotControlled:
		return LOCTEXT("NotControlled_Tooltip", "Item is not under version control.");
	case EAnchorpointState::Missing:
		return LOCTEXT("Missing_Tooltip", "Item is missing (e.g., you moved or deleted it without using Git). This also indicates that a directory is incomplete (a checkout or update was interrupted).");
	case EAnchorpointState::OutDated:
		return LOCTEXT("NotCurrent_Tooltip", "The file(s) are not at the head revision");
	case EAnchorpointState::Locked:
		return FText::Format(LOCTEXT("CheckedOutOther_Tooltip", "Checked out by: {0}"), FText::FromString(OtherUserCheckedOut));
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
	return State == EAnchorpointState::Added
		|| State == EAnchorpointState::Deleted
		|| State == EAnchorpointState::Missing
		|| State == EAnchorpointState::Modified
		|| State == EAnchorpointState::Renamed;
}

bool FAnchorpointControlState::CanCheckout() const
{
	return false;
}

bool FAnchorpointControlState::IsCheckedOut() const
{
	return IsSourceControlled();
}

bool FAnchorpointControlState::IsCheckedOutOther(FString* Who) const
{
	if (Who != nullptr)
	{
		*Who = OtherUserCheckedOut;
	}

	return State == EAnchorpointState::Locked;
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
	return State != EAnchorpointState::NotControlled
		&& State != EAnchorpointState::Ignored
		&& State != EAnchorpointState::Unknown;
}

bool FAnchorpointControlState::IsAdded() const
{
	return State == EAnchorpointState::Added;
}

bool FAnchorpointControlState::IsDeleted() const
{
	return State == EAnchorpointState::Deleted || State == EAnchorpointState::Missing;
}

bool FAnchorpointControlState::IsIgnored() const
{
	return State == EAnchorpointState::Ignored;
}

bool FAnchorpointControlState::CanEdit() const
{
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
	//@note check FGitSourceControlState::IsModified for reference, it explains why we can't do State == EAnchorpointState::Modified

	return State == EAnchorpointState::Added
		|| State == EAnchorpointState::Deleted
		|| State == EAnchorpointState::Modified
		|| State == EAnchorpointState::Renamed
		|| State == EAnchorpointState::Copied
		|| State == EAnchorpointState::Conflicted
		|| State == EAnchorpointState::Missing;
}

bool FAnchorpointControlState::CanAdd() const
{
	return State == EAnchorpointState::NotControlled;
}

bool FAnchorpointControlState::IsConflicted() const
{
	return State == EAnchorpointState::Conflicted;
}

bool FAnchorpointControlState::CanRevert() const
{
	return State == EAnchorpointState::Added
		|| State == EAnchorpointState::Deleted
		|| State == EAnchorpointState::Missing
		|| State == EAnchorpointState::Modified
		|| State == EAnchorpointState::Renamed;
}

#undef LOCTEXT_NAMESPACE