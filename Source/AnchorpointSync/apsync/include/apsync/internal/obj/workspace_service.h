#ifndef INCLUDE_OBJ_WORKSPACE_SERVICE
#define INCLUDE_OBJ_WORKSPACE_SERVICE

#include "apsync/internal/models/access_rule.h"
#include "apsync/internal/obj/obj_base_service.h"
#include "apsync/internal/sync_context.h"
#include "apsync/sync_global.h"
#include "clientd/groups.grpc.pb.h"
//#include "clientd/workspaces.grpc.pb.h"

namespace apsync {
class SYNC_EXPORT WorkspaceService: public ObjBaseService
{
public:
    explicit WorkspaceService(std::shared_ptr<SyncContext> context);

    Result<std::string> createGroup(const std::string& name, const std::string& kind,
                                    const std::string& workspaceId);

    Result<void> deleteGroup(const std::string& groupId, const std::string& workspaceId);

    Result<void> addMemberToGroup(const std::string& groupId, const std::string& workspaceId,
                                  const std::string& userId, AccessRule::Level level);

private:
    std::shared_ptr<clientd::WorkspaceGroupService::Stub> _groupStub;
    // std::shared_ptr<clientd::WorkspaceService::Stub> _stub;
};
} // namespace apsync

#endif /* INCLUDE_OBJ_WORKSPACE_SERVICE */
