#ifndef INCLUDE_INTERNAL_SERVICE_P_API
#define INCLUDE_INTERNAL_SERVICE_P_API

#include "apsync/internal/sync_context.h"
#include "apsync/sync_global.h"

namespace apsync {
struct ApiImpl {
    std::shared_ptr<SyncContext> context;
};
}// namespace apsync

#endif /* INCLUDE_INTERNAL_SERVICE_P_API */
