//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT ChangesApi
{
public:
    explicit ChangesApi(std::shared_ptr<Api> api);

    Result<void> listen(
        const Context& ctx,
        const ChangeCallback& callback
    );

private:
    std::shared_ptr<class ChangesApiImpl> _impl;
};

} // namespace apsync
//$generated