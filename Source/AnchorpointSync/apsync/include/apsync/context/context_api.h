//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT ContextApi
{
public:
    explicit ContextApi(std::shared_ptr<Api> api);

    Result<Context> create(
    );

    Result<void> cancel(
        const Context& ctx
    );

    Result<void> cancelAll(
    );

private:
    std::shared_ptr<class ContextApiImpl> _impl;
};

} // namespace apsync
//$generated