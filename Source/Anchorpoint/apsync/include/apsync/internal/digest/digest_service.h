#pragma once

#include <mutex>
#include <queue>
#include <thread>

#include "apsync/internal/grpc_base_service.h"
#include "apsync/service/api.h"
#include "apsync/sync_global.h"
#include "clientd/digest.grpc.pb.h"

namespace apsync {

class DigestService : public GrpcBaseService
{
public:
    explicit DigestService(std::shared_ptr<SyncContext> context);

    Result<string> calculate(const string& path);

    Result<bool> match(const string& path, const string& digest);

private:
    unique_ptr<clientd::DigestService::Stub> _stub;
};

} // namespace apsync
