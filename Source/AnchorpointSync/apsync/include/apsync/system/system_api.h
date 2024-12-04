//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT SystemApi
{
public:
    explicit SystemApi(std::shared_ptr<Api> api);

    Result<std::string> getVersion(
    );

    Result<void> setOfflineMode(
        bool offline,
        const UserNew& user
    );

    Result<bool> getOfflineMode(
    );

    Result<void> shutdown(
    );

private:
    std::shared_ptr<class SystemApiImpl> _impl;
};

} // namespace apsync
//$generated