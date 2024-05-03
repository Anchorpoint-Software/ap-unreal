//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT WorkspacesApi
{
public:
    explicit WorkspacesApi(std::shared_ptr<Api> api);

    Result<std::vector<UserWorkspace>> list(
    );

    Result<void> deleteWorkspace(
        const std::string& workspaceId
    );

    Result<void> setName(
        const std::string& workspaceId,
        const std::string& name
    );

    Result<int64> getAccess(
        const std::string& workspaceId
    );

    Result<std::vector<WorkspaceMember>> listMembers(
        const std::string& workspaceId
    );

    Result<void> addMember(
        const std::string& workspaceId,
        const std::string& userId,
        int64 level
    );

    Result<void> setMemberLevel(
        const std::string& workspaceId,
        const std::string& userId,
        int64 level
    );

    Result<void> removeMember(
        const std::string& workspaceId,
        const std::string& userId
    );

    Result<std::vector<Invite>> listInvites(
        const std::string& workspaceId
    );

    Result<Invite> addInvite(
        const std::string& workspaceId,
        const std::string& email,
        int64 level,
        const std::string& message,
        const std::string& inviterName,
        const std::string& workspaceName,
        const std::string& projectName
    );

    Result<void> removeInvite(
        const std::string& workspaceId,
        const std::string& inviteId
    );

private:
    std::shared_ptr<class WorkspacesApiImpl> _impl;
};

} // namespace apsync
//$generated