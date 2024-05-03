//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT TimelineApi
{
public:
    explicit TimelineApi(std::shared_ptr<Api> api);

    Result<void> addChannel(
        const apsync::ProjectObject& project,
        const TimelineChannel& channel
    );

    Result<void> removeChannel(
        const apsync::ProjectObject& project,
        const std::string& channelId
    );

    Result<std::vector<TimelineChannel>> getChannels(
        const apsync::ProjectObject& project
    );

    Result<std::optional<TimelineChannel>> getChannel(
        const apsync::ProjectObject& project,
        const std::string& channelId
    );

    Result<void> updateChannel(
        const apsync::ProjectObject& project,
        const TimelineChannel& channel
    );

private:
    std::shared_ptr<class TimelineApiImpl> _impl;
};

} // namespace apsync
//$generated