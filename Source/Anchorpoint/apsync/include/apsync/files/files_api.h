//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT FilesApi
{
public:
    explicit FilesApi(std::shared_ptr<Api> api);

    Result<void> listFiles(
        const Context& ctx,
        const std::string& path,
        const ListFilesOptions& options,
        const ListFilesDelegate& delegate
    );

    Result<FileEntryWithAttributes> getFile(
        const std::string& path
    );

private:
    std::shared_ptr<class FilesApiImpl> _impl;
};

} // namespace apsync
//$generated