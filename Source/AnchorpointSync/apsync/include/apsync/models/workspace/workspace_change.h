#ifndef WORKSPACE_CHANGE_H
#define WORKSPACE_CHANGE_H

#include "apsync/sync_global.h"

namespace apsync {

/*enum class WorkspaceChangeType {
    WorkspaceChange,
    WorkspaceMemberChange,
    GroupChange,
    GroupMemberChange,
    AccountInviteChange,
    WorkspaceInviteChange,
    UserProfileChange,
    SubscriptionChange,
    TrialChange,
};

struct SYNC_EXPORT WorkspaceChangeBase {
    std::string sourceTag;
    WorkspaceChangeType type;
};

using WorkspaceChangeBaseRef = std::shared_ptr<WorkspaceChangeBase>;

struct SYNC_EXPORT WorkspaceChange: WorkspaceChangeBase {
    WorkspaceChange() { type = WorkspaceChangeType::WorkspaceChange; }
    std::string workspaceId;
};

using WorkspaceChangeRef = std::shared_ptr<WorkspaceChange>;

struct SYNC_EXPORT WorkspaceMemberChange: WorkspaceChangeBase {
    WorkspaceMemberChange() { type = WorkspaceChangeType::WorkspaceMemberChange; }
    std::string workspaceId;
    std::string memberId;
};

using WorkspaceMemberChangeRef = std::shared_ptr<WorkspaceMemberChange>;

struct SYNC_EXPORT GroupChange: WorkspaceChangeBase {
    GroupChange() { type = WorkspaceChangeType::GroupChange; }
    std::string workspaceId;
    std::string groupId;
};

using GroupChangeRef = std::shared_ptr<GroupChange>;

struct SYNC_EXPORT GroupMemberChange: WorkspaceChangeBase {
    GroupMemberChange() { type = WorkspaceChangeType::GroupMemberChange; }
    std::string workspaceId;
    std::string groupId;
    std::string memberId;
};

using GroupMemberChangeRef = std::shared_ptr<GroupMemberChange>;

struct SYNC_EXPORT AccountInviteChange: WorkspaceChangeBase {
    AccountInviteChange() { type = WorkspaceChangeType::AccountInviteChange; }
};

using AccountInviteChangeRef = std::shared_ptr<AccountInviteChange>;

struct SYNC_EXPORT WorkspaceInviteChange: WorkspaceChangeBase {
    WorkspaceInviteChange() { type = WorkspaceChangeType::WorkspaceInviteChange; }
    std::string workspaceId;
};

using WorkspaceInviteChangeRef = std::shared_ptr<WorkspaceInviteChange>;

struct SYNC_EXPORT UserProfileChange: WorkspaceChangeBase {
    UserProfileChange() { type = WorkspaceChangeType::UserProfileChange; }
    std::string userId;
};
using UserProfileChangeRef = std::shared_ptr<UserProfileChange>;

struct SYNC_EXPORT SubscriptionChange: WorkspaceChangeBase {
    SubscriptionChange() { type = WorkspaceChangeType::SubscriptionChange; }
    std::string subId;
};
using SubscriptionChangeRef = std::shared_ptr<SubscriptionChange>;

struct SYNC_EXPORT TrialChange: WorkspaceChangeBase {
    TrialChange() { type = WorkspaceChangeType::TrialChange; }
    std::string trialId;
};
using TrialChangeRef = std::shared_ptr<TrialChange>;
*/
} // namespace apsync

#endif // WORKSPACE_CHANGE_H
