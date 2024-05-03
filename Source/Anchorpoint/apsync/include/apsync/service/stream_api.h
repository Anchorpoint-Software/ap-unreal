#ifndef STREAM_API_H
#define STREAM_API_H

#include <unordered_map>

#include "apsync/models/base_object.h"
#include "apsync/models/stream/stream_update_model.h"
#include "apsync/sync_global.h"

namespace apsync {
/*
class Api;
class StreamApiImpl;
class SYNC_EXPORT StreamApi
{
public:
    explicit StreamApi(std::shared_ptr<Api> api);

    Result<void> listen(StreamUpdateCallback callback);

    void cancel();

private:
    std::shared_ptr<StreamApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
};*/
} // namespace apsync

#endif // STREAM_API_H
