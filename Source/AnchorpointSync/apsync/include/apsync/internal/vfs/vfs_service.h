#ifndef VFS_SERVICE_H
#define VFS_SERVICE_H

#include "apsync/internal/obj/obj_base_service.h"
#include "apsync/internal/sync_context.h"
#include "apsync/sync_global.h"
#include "clientd/vfs.grpc.pb.h"

#include "apsync/internal/models/vfs/vfs_local_path.h"
#include "apsync/internal/models/vfs/vfs_remote_path.h"
#include "apsync/internal/models/vfs/vfs_repo_target.h"

#include <unordered_map>

namespace apsync {
class SYNC_EXPORT VfsService : public ObjBaseService
{
public:
    explicit VfsService(std::shared_ptr<SyncContext> context);

    Result<std::vector<std::optional<std::string>>> put(const VfsRepoTarget& target,
                                                        const std::vector<VfsLocalPath>& localPaths);
    Result<std::unordered_map<std::string, std::string>> get(
        const VfsRepoTarget& target,
        const std::vector<VfsRemotePath>& remotePaths,
        std::function<bool(const std::unordered_map<std::string, std::string>& cachedPaths)>
            callback);
    Result<void> remove(const VfsRepoTarget& target, const std::vector<VfsRemotePath>& remotePaths);

    Result<void> copyAll(const std::string& srcObjectId,
                         const std::string& dstObjectId,
                         const VfsRepoTarget& target);

private:
    std::shared_ptr<clientd::VFSService::Stub> _stub;
};
} // namespace apsync

#endif // VFS_SERVICE_H
