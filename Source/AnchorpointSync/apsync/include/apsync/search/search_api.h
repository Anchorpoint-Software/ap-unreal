//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT SearchApi
{
public:
    explicit SearchApi(std::shared_ptr<Api> api);

    Result<void> byQuery(
        const Context& ctx,
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const std::string& rootPath,
        const std::string& query,
        const SearchOptions& options,
        const SearchDelegate& delegate
    );

    Result<void> byQueryOnRemote(
        const Context& ctx,
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const std::string& rootPath,
        const std::vector<std::string>& filePaths,
        const std::vector<std::string>& folderPaths,
        const std::string& query,
        const SearchOptions& options,
        const SearchDelegate& delegate
    );

private:
    std::shared_ptr<class SearchApiImpl> _impl;
};

} // namespace apsync
//$generated