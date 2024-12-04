#ifndef INCLUDE_GRPC_BASE_SERVICE
#define INCLUDE_GRPC_BASE_SERVICE

#include "apsync/internal/sync_context.h"
#include "apsync/sync_global.h"
#include "clientd/session.pb.h"

namespace apsync {

void setSessionMetadata(grpc::ClientContext& grpcContext, const std::shared_ptr<SyncContext>& context);

bool isOfflineModeStatus(const grpc::Status& status);


class SYNC_EXPORT GrpcBaseService
{
public:
    GrpcBaseService(std::shared_ptr<SyncContext> context);

protected:
    Result<void> checkStatus(grpc::Status status) const;

    std::shared_ptr<SyncContext> _context;
};
} // namespace apsync

#endif /* INCLUDE_GRPC_BASE_SERVICE */
