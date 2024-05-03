//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT CacheApi
{
public:
    explicit CacheApi(std::shared_ptr<Api> api);

    Result<void> bind(
        const Context& ctx
    );

    Result<std::any> get(
        const Context& ctx,
        const std::string& objectId
    );

    Result<void> release(
        const Context& ctx
    );

private:
    std::shared_ptr<class CacheApiImpl> _impl;
};

} // namespace apsync
//$generated