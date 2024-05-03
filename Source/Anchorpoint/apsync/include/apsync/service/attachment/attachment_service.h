#ifndef ATTACHMENT_SERVICE_H
#define ATTACHMENT_SERVICE_H

#include "apsync/models/file_object.h"
#include "apsync/models/journal/journal_entry.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class AttachmentServiceImpl;
class SYNC_EXPORT AttachmentService
{
public:
    explicit AttachmentService(std::shared_ptr<Api> api);

    Result<std::vector<AttachmentDocument>> attach(
        const std::optional<ProjectObject>& projectObject, const std::string& objectId,
        const std::string& workspaceId, const std::string& folderPath,
        const std::vector<std::string>& paths, bool makeLocalToProject);

    Result<AttachmentDocument> getAttachment(const std::string& objectId,
                                             const std::string& attachmentId,
                                             const std::string& workspaceId) const;

    Result<std::vector<AttachmentDocument>> getAttachments(const BaseObject& object) const;

    Result<void> updateAttachment(const AttachmentDocument& attachment) const;

    Result<void> promoteExternalAttachment(const std::optional<ProjectObject>& projectObject,
                                           const std::string& workspaceId,
                                           const ExternalFileAttachmentRef& attachment,
                                           AttachmentDocument attachmentDocument,
                                           const std::string& targetFolder) const;

    Result<void> deleteAttachment(const AttachmentDocument& attachment) const;

    Result<AttachmentRef> resolveAttachment(const std::optional<ProjectObject>& projectObject,
                                            const BaseObject& object, const std::string& folderPath,
                                            const AttachmentDocument& attachment,
                                            bool resolveSubEntries) const;

    bool attachmentExists(const std::string& objectId, const std::string& attachmentId,
                          const std::string& workspaceId) const;

    static Result<std::string> getAttachmentDir(std::string folderPath, bool create);

    static std::string shortenId(const std::string& id);

    static std::optional<std::string> resolveExternalFilepath(const std::string& objectId,
                                                              const std::string& folderPath,
                                                              const std::string& filename);


private:
    std::shared_ptr<Api> _api;
    std::shared_ptr<AttachmentServiceImpl> _pimpl;
};
} // namespace apsync

#endif // ATTACHMENT_SERVICE_H
