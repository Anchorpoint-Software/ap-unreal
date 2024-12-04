//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT GroupsApi
{
public:
    explicit GroupsApi(std::shared_ptr<Api> api);

    Result<std::vector<UserGroup>> list(
        const std::string& workspaceId
    );

    Result<std::vector<Group>> listAll(
        const std::string& workspaceId
    );

    Result<Group> create(
        const std::string& workspaceId,
        const std::string& name,
        const std::string& kind
    );

    Result<Group> get(
        const std::string& workspaceId,
        const std::string& groupId
    );

    Result<void> remove(
        const std::string& workspaceId,
        const std::string& groupId
    );

    Result<std::vector<GroupMember>> listMembers(
        const std::string& workspaceId,
        const std::string& groupId
    );

    Result<void> addMember(
        const std::string& workspaceId,
        const std::string& groupId,
        const std::string& userId,
        int64 level
    );

    Result<void> setMemberLevel(
        const std::string& workspaceId,
        const std::string& groupId,
        const std::string& userId,
        int64 level
    );

    Result<void> removeMember(
        const std::string& workspaceId,
        const std::string& groupId,
        const std::string& userId
    );

private:
    std::shared_ptr<class GroupsApiImpl> _impl;
};

} // namespace apsync
//$generated