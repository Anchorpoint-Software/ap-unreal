//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT IpcApi
{
public:
    explicit IpcApi(std::shared_ptr<Api> api);

    Result<IpcSubscription> subscribe(
        const std::string& topic
    );

    Result<void> unsubscribe(
        const IpcSubscription& sub
    );

    Result<bool> hasMessages(
        const IpcSubscription& sub
    );

    Result<std::optional<IpcMessage>> tryGetMessage(
        const IpcSubscription& sub
    );

    Result<void> publish(
        const IpcMessage& msg
    );

private:
    std::shared_ptr<class IpcApiImpl> _impl;
};

} // namespace apsync
//$generated