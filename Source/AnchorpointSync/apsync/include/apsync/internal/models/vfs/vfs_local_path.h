#ifndef VFS_LOCAL_PATH_H
#define VFS_LOCAL_PATH_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT VfsLocalPath
{
    std::string objectId;
    std::string remotePath;
    std::string localPath;
};
} // namespace apsync

#endif // VFS_LOCAL_PATH_H
