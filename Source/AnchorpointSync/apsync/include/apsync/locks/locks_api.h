//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT LocksApi
{
public:
    explicit LocksApi(std::shared_ptr<Api> api);

    Result<std::vector<apsync::LockObject>> getLocks(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project
    );

    Result<std::vector<apsync::LockObject>> getLocksById(
        const std::string& workspaceId,
        const std::vector<std::string>& ids
    );

    Result<std::optional<apsync::LockObject>> getLock(
        const apsync::BaseObject& object
    );

    Result<std::string> getLockedPath(
        const apsync::LockObject& lock,
        const ProjectObject& project
    );

    Result<apsync::LockObject> mapLock(
        const std::string& id,
        const std::string& workspaceId,
        const std::string& resource,
        const std::string& projectId,
        const uint64& createdAt,
        const std::vector<apsync::AccessRule>& access,
        const std::string& props
    );

    Result<std::vector<apsync::LockObject>> lock(
        const std::vector<apsync::BaseObject>& objects,
        const ProjectObject& project,
        const std::optional<std::string>& description,
        const std::unordered_map<std::string,std::string>& data
    );

    Result<void> unlock(
        const std::vector<apsync::LockObject>& locks,
        const std::string& workspaceId
    );

    Result<void> update(
        const std::vector<apsync::LockObject>& locks,
        const std::string& workspaceId
    );

private:
    std::shared_ptr<class LocksApiImpl> _impl;
};

} // namespace apsync
//$generated