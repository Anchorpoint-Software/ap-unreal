#ifndef INCLUDE_OBJ_OBJ_BASE_SERVICE
#define INCLUDE_OBJ_OBJ_BASE_SERVICE

#include "apsync/internal/grpc_base_service.h"
#include "apsync/internal/models/access_rule.h"
#include "apsync/internal/sync_context.h"
#include "apsync/sync_global.h"
#include "core/access.grpc.pb.h"

namespace apsync {
class SYNC_EXPORT ObjBaseService: public GrpcBaseService
{
public:
    ObjBaseService(std::shared_ptr<SyncContext> context);
};

void marshalAccess(google::protobuf::RepeatedPtrField<::core::AccessRule>* rules,
                          const std::vector<AccessRule>& accessRules);

std::vector<AccessRule> unmarshalAccess(
    const google::protobuf::RepeatedPtrField<::core::AccessRule>& rules);

void marshalTags(google::protobuf::Map<std::string, std::string>* map,
                        const std::unordered_map<std::string, std::string>& tags);

std::unordered_map<std::string, std::string> unmarshalTags(
    const google::protobuf::Map<std::string, std::string>& map);

}// namespace apsync

#endif /* INCLUDE_OBJ_OBJ_BASE_SERVICE */
