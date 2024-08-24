#ifndef VFS_REMOTE_PATH_H
#define VFS_REMOTE_PATH_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT VfsRemotePath
{
    std::string objectId;
    std::string remotePath;
};
} // namespace apsync

#endif // VFS_REMOTE_PATH_H
