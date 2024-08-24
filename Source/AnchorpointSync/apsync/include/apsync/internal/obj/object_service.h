#ifndef INCLUDE_OBJ_OBJECT_SERVICE
#define INCLUDE_OBJ_OBJECT_SERVICE

#include "apsync/internal/models/object.h"
#include "apsync/internal/models/object_tag.h"
#include "apsync/internal/obj/obj_base_service.h"
#include "apsync/internal/sync_context.h"
#include "apsync/sync_global.h"
#include "clientd/objects.grpc.pb.h"

namespace apsync {

class ObjectUpdateCommands;

class SYNC_EXPORT ObjectService: public ObjBaseService
{
public:
    using UpdateFunc = std::function<void(ObjectUpdateCommands& commands)>;

    explicit ObjectService(std::shared_ptr<SyncContext> context);

    Result<std::vector<Object>> search(const std::vector<std::string>& kinds,
                                       const std::vector<std::string>& resources,
                                       const std::string& workspaceId);

    Result<std::vector<Object>> search(const std::vector<std::string>& kinds,
                                       const std::vector<std::string>& resources,
                                       const tag::Tags& tags, const std::string& workspaceId);

    Result<std::vector<Object>> search(const std::vector<std::string>& kinds,
                                       const std::vector<std::string>& resources,
                                       const tag::Tags& tags,
                                       const std::unordered_map<std::string, std::string> props,
                                       const std::string& workspaceId);

    Result<Object> get(const std::string& workspaceId, const std::string& objectId);

    Result<std::vector<Object>> get(const std::string& workspaceId,
                                    const std::vector<std::string>& objectIds);

    Result<std::vector<Object>> update(const std::string& workspaceId, UpdateFunc f);

    Result<Object> create(const std::string& workspaceId, const Object& object,
                          bool exclusive = false);

    Result<vector<Object>> create(const std::string& workspaceId, const vector<Object>& objects,
                                  bool exclusive = false);

    Result<void> setResource(const std::string& workspaceId, const std::string& objectId,
                             const std::string& resource);

    Result<void> setProps(const std::string& workspaceId, const std::string& objectId,
                          const std::string& propsData);

    Result<void> setTags(const std::string& workspaceId, const std::string& objectId,
                         const std::unordered_map<std::string, std::string>& tags);

    Result<void> setAccess(const std::string& workspaceId, const std::string& objectId,
                           const std::vector<AccessRule>& access);

    Result<void> erase(const std::string& workspaceId, const std::string& objectId,
                       bool keepDocuments = false);
    Result<void> erase(const std::string& workspaceId, const vector<string>& objectIds,
                       bool keepDocuments = false);

    Result<void> removeTag(const std::string& objectId, const std::string& workspaceId,
                           const std::string& tagKey);

    Result<void> sync(const std::string& workspaceId, bool resync = false);

    Result<void> dropLocal(const std::string& workspaceId);

    Result<void> rescanAccess(const std::string& workspaceId, const std::string& accessId);

    Result<void> syncOffline(const std::string& workspaceId);

    static std::vector<std::string> getAllKinds();

private:
    std::unique_ptr<clientd::ObjectService::Stub> _stub;
};


class ObjectUpdateCommands
{
public:
    void create(const Object& object, bool exclusive = false);

    void setResource(const std::string& objectId, const std::string& resource);

    void setProps(const std::string& objectId, const std::string& propsData);

    void setTags(const std::string& objectId,
                 const std::unordered_map<std::string, std::string>& tags);

    void setAccess(const std::string& objectId, const std::vector<AccessRule>& access);

    void erase(const std::string& objectId, bool keepDocuments = false);

private:
    clientd::UpdateObjectsRequest* _request = nullptr;

    friend class ObjectService;
};

Object unmarshalObject(const core::Object& object);

} // namespace apsync

#endif /* INCLUDE_OBJ_OBJECT_SERVICE */
