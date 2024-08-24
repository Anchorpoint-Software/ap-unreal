#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include "apsync/internal/obj/obj_base_service.h"
#include "apsync/models/event/event_model.h"
#include "clientd/events.grpc.pb.h"

namespace apsync {
class SYNC_EXPORT EventService: public ObjBaseService
{
public:
    explicit EventService(std::shared_ptr<SyncContext> context);

    Result<Event> create(const std::string& workspaceId, const Event& event);

    Result<std::vector<Event>> search(const std::optional<std::string>& subjectId,
                                      const std::vector<std::string>& kinds,
                                      const std::vector<std::string>& actions,
                                      const std::string& workspaceId);

    Result<void> sync(const std::string& workspaceId);

    Result<void> syncOffline(const std::string& workspaceId);

    static Event unmarshalEvent(const core::Event& event);

private:
    std::shared_ptr<clientd::EventService::Stub> _stub;
};
} // namespace apsync
#endif // EVENT_SERVICE_H
