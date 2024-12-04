#ifndef STREAM_SERVICE_H
#define STREAM_SERVICE_H

#include <shared_mutex>

#include "apsync/internal/models/object.h"
#include "apsync/internal/obj/obj_base_service.h"
#include "apsync/internal/sync_context.h"
#include "apsync/models/base_object.h"
#include "apsync/models/stream/stream_update_model.h"
#include "apsync/sync_global.h"
/*
namespace apsync {
class StreamServiceImpl;
class SYNC_EXPORT StreamService: public ObjBaseService
{
public:
    explicit StreamService(std::shared_ptr<SyncContext> context);

    Result<void> listen(StreamUpdateCallback callback);

    void cancel();

private:
    std::shared_ptr<StreamServiceImpl> _impl;
};
} // namespace apsync
*/
#endif /* STREAM_SERVICE_H */
