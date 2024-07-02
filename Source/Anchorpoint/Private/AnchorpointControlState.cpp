// Some copyright should be here...

#include "AnchorpointControlState.h"

#include "ISourceControlModule.h"
#include "Styling/AppStyle.h"
#include "RevisionControlStyle/RevisionControlStyle.h"

#define LOCTEXT_NAMESPACE "PlasticSourceControl.State"

int32 FAnchorpointControlState::GetHistorySize() const
{
	return 0;
}

TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> FAnchorpointControlState::GetHistoryItem(int32 HistoryIndex) const
{
	return nullptr;
}

TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> FAnchorpointControlState::FindHistoryRevision(int32 RevisionNumber) const
{
	return nullptr;
}

TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> FAnchorpointControlState::FindHistoryRevision(const FString& InRevision) const
{
	return nullptr;
}

TSharedPtr<class ISourceControlRevision, ESPMode::ThreadSafe> FAnchorpointControlState::GetCurrentRevision() const
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
	return FSlateIcon(FRevisionControlStyleManager::GetStyleSetName(), "RevisionControl.NotAtHeadRevision");
}

#endif // SOURCE_CONTROL_WITH_SLATE

FText FAnchorpointControlState::GetDisplayName() const
{
	return INVTEXT("Locked by Anchorpoint");
}

FText FAnchorpointControlState::GetDisplayTooltip() const
{
	return INVTEXT("Locked by Anchorpoint tip");
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
	return false;
}

bool FAnchorpointControlState::CanCheckout() const
{
	return false;
}

bool FAnchorpointControlState::IsCheckedOut() const
{
	return true;
}

bool FAnchorpointControlState::IsCheckedOutOther(FString* Who) const
{
	return true;
}

bool FAnchorpointControlState::IsCheckedOutInOtherBranch(const FString& CurrentBranch /* = FString() */) const
{
	return false;
}

/** Get whether this file is modified in a different branch */
bool FAnchorpointControlState::IsModifiedInOtherBranch(const FString& CurrentBranch /* = FString() */) const
{
	return false;
}

/** Get head modification information for other branches
 * @returns true with parameters populated if there is a branch with a newer modification (edit/delete/etc)
*/
bool FAnchorpointControlState::GetOtherBranchHeadModification(FString& HeadBranchOut, FString& ActionOut, int32& HeadChangeListOut) const
{
	return true;
}

bool FAnchorpointControlState::IsCurrent() const
{
	return true;
}

bool FAnchorpointControlState::IsSourceControlled() const
{
	return true;
}

bool FAnchorpointControlState::IsAdded() const
{
	return false;
}

bool FAnchorpointControlState::IsDeleted() const
{
	return false;
}

bool FAnchorpointControlState::IsIgnored() const
{
	return false;
}

bool FAnchorpointControlState::CanEdit() const
{
	return false;
}

bool FAnchorpointControlState::CanDelete() const
{
	return false;
}

bool FAnchorpointControlState::IsUnknown() const
{
	return false;
}

bool FAnchorpointControlState::IsModified() const
{
	return false;
}


bool FAnchorpointControlState::CanAdd() const
{
	return false;
}

bool FAnchorpointControlState::IsConflicted() const
{
	return false;
}

bool FAnchorpointControlState::CanRevert() const
{
	return false;
}

#undef LOCTEXT_NAMESPACE
