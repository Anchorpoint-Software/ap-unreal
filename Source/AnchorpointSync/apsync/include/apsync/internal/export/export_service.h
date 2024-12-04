#ifndef EXPORT_SERVICE_H
#define EXPORT_SERVICE_H

#include <optional>

#include "apsync/internal/models/object_tag.h"
#include "apsync/internal/obj/obj_base_service.h"
#include "clientd/export.grpc.pb.h"

namespace apsync {
struct ExportObjectInclude {
    std::optional<std::vector<std::string>> kinds;
    std::optional<std::vector<std::string>> resources;
    std::optional<tag::Tags> tags;
};

struct ExportDocumentInclude {
    std::string repo;
    std::optional<tag::Tags> tags;
};


class SYNC_EXPORT ExportService: public ObjBaseService
{
public:
    explicit ExportService(std::shared_ptr<SyncContext> context);

    Result<clientd::ExportedWorkspace> exportWorkspace(
        const std::string& workspaceId, const std::vector<std::string>& groupIds,
        const std::vector<ExportObjectInclude>& objects,
        const std::vector<ExportDocumentInclude>& docs, const std::vector<std::string>& userIds);
    Result<void> importWorkspace(const std::string& workspaceId,
                                 clientd::ExportedWorkspace* exportedWorkspace);

private:
    std::shared_ptr<clientd::ExportService::Stub> _stub;
};
} // namespace apsync
#endif // EXPORT_SERVICE_H
