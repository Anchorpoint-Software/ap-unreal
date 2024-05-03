#ifndef APSYNC_INTERNAL_OBJ_DOCUMENT_SERVICE
#define APSYNC_INTERNAL_OBJ_DOCUMENT_SERVICE

#include "apsync/internal/models/document.h"
#include "apsync/internal/models/document_repo_target.h"
#include "apsync/internal/models/document_target.h"
#include "apsync/internal/models/object_tag.h"
#include "apsync/internal/obj/obj_base_service.h"
#include "apsync/internal/sync_context.h"
#include "apsync/sync_global.h"
#include "clientd/documents.grpc.pb.h"

namespace apsync {

class DocumentUpdateCommands;

class SYNC_EXPORT DocumentService: public ObjBaseService
{
public:
    using UpdateFunc = std::function<void(DocumentUpdateCommands& commands)>;

    explicit DocumentService(std::shared_ptr<SyncContext> context);

    Result<std::vector<Document>> search(
        const std::string& workspaceId, const std::string& repo,
        const std::optional<std::string>& objectId,
        const std::optional<std::vector<std::pair<std::string, std::string>>>& query = std::nullopt,
        const std::optional<tag::Tags>& tags = std::nullopt);

    Result<Document> get(const std::string& workspaceId, const std::string& repo,
                         const std::string& docId);

    Result<std::vector<Document>> get(const std::string& workspaceId, const std::string& repo,
                                      const std::vector<std::string>& docIds);

    Result<std::vector<Document>> update(const std::string& workspaceId, const std::string& repo,
                                         UpdateFunc f);

    Result<Document> create(const std::string& workspaceId, const std::string& repo,
                            const Document& doc);
    Result<vector<Document>> create(const std::string& workspaceId, const std::string& repo,
                                    const vector<Document>& docs);

    Result<void> setContent(const std::string& workspaceId, const std::string& repo,
                            const std::string& docId, const std::string& contentData);

    Result<void> setAccess(const std::string& workspaceId, const std::string& repo,
                           const std::string& docId, const std::vector<AccessRule>& access);

    Result<void> erase(const std::string& workspaceId, const std::string& repo,
                       const std::string& docId);

    Result<void> sync(const DocumentRepoTarget& target, bool resync = false);
    Result<void> dropLocal(const DocumentRepoTarget& target);
    Result<void> syncOffline(const std::string& workspaceId);

    Result<void> rescanAccess(const DocumentRepoTarget& target, const std::string& accessId);

    static std::vector<std::string> getAllRepos();

private:
    std::unique_ptr<clientd::DocumentService::Stub> _stub;
};

class DocumentUpdateCommands
{
public:
    void create(const Document& doc);

    void setContent(const std::string& docId, const std::string& contentData);

    void setAccess(const std::string& docId, const std::vector<AccessRule>& access);

    void erase(const std::string& docId);

private:
    clientd::UpdateDocumentsRequest* _request = nullptr;

    friend class DocumentService;
};

Document unmarshalDocument(const core::Document& doc);

} // namespace apsync

#endif /* APSYNC_INTERNAL_OBJ_DOCUMENT_SERVICE */
