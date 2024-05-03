//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT ColorApi
{
public:
    explicit ColorApi(std::shared_ptr<Api> api);

    Result<void> updateConfig(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project,
        const ColorConfig& config
    );

    Result<ColorConfig> getConfig(
        const std::string& workspaceId,
        const std::optional<ProjectObject>& project
    );

    Result<std::string> getConfigHash(
        const ColorConfig& config
    );

private:
    std::shared_ptr<class ColorApiImpl> _impl;
};

} // namespace apsync
//$generated