//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT LayoutApi
{
public:
    explicit LayoutApi(std::shared_ptr<Api> api);

    Result<Layout> getLayout(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const apsync::Url& localPath
    );

    Result<Layout> identifyLayout(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const apsync::Url& localPath
    );

    Result<Layout> upsertLayout(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const Layout& layout
    );

    Result<Layout> insertLayoutEntry(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const apsync::Url& localPath,
        const LayoutEntry& entry,
        int64 insertIndex
    );

    Result<Layout> moveLayoutEntry(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const apsync::Url& localPath,
        const std::string& fromId,
        const std::string& toId
    );

    Result<Layout> removeLayoutEntry(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const apsync::Url& localPath,
        const std::string& layoutEntryId
    );

private:
    std::shared_ptr<class LayoutApiImpl> _impl;
};

} // namespace apsync
//$generated