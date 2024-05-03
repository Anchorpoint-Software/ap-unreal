#ifndef VFS_REPO_TARGET_H
#define VFS_REPO_TARGET_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT VfsRepoTarget {
    std::string workspaceId;
    std::string repoName = "media";
};
} // namespace apsync

#endif // VFS_REPO_TARGET_H
