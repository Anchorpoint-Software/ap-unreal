#ifndef MENTION_API_H
#define MENTION_API_H

#include "apsync/models/attributes/database_object.h"
#include "apsync/models/event/event_model.h"
#include "apsync/models/file_object.h"
#include "apsync/models/mention/mention.h"
#include "apsync/sync_global.h"

namespace apsync {

class Api;
class MentionApiImpl;
class SYNC_EXPORT MentionApi
{
public:
    explicit MentionApi(std::shared_ptr<Api> api);

    Result<MentionBaseRef> createMentionReview(const std::string& workspaceId,
                                               const std::optional<ProjectObject>& projectOpt,
                                               const std::vector<std::string>& userIds,
                                               const FileObject& fileObject,
                                               const std::string& reviewId);

    Result<MentionBaseRef> createMentionVersion(const std::string& workspaceId,
                                                const std::optional<ProjectObject>& projectOpt,
                                                const std::vector<std::string>& userIds,
                                                const FileObject& fileObject,
                                                const std::string& versionEntryId);

    Result<MentionBaseRef> createMentionTaskEntry(const std::string& workspaceId,
                                                  const std::optional<ProjectObject>& projectOpt,
                                                  const std::vector<std::string>& userIds,
                                                  const std::string& taskId,
                                                  const std::string& taskJournalId);

    Result<MentionBaseRef> createMentionUserAttribute(
        const std::string& workspaceId, const std::optional<ProjectObject>& projectOpt,
        const std::vector<std::string>& userIds, const DatabaseObject& database,
        const std::string& objectId);

    Result<MentionRead> mentionRead(const std::string& subjectId, const std::string& workspaceId,
                                    const std::optional<ProjectObject>& projectOpt);

    Result<std::vector<MentionBaseRef>> getAllMentions(
        const std::string& workspaceId, const std::vector<ProjectObject>& projects) const;

    Result<std::optional<MentionBaseRef>> getMention(
        const std::string& workspaceId, const std::string& subjectId, const std::string& eventId,
        const std::optional<ProjectObject>& projectOpt) const;

    Result<std::optional<std::string>> getProjectIdFromMention(const std::string& workspaceId,
                                                               const std::string& subjectId,
                                                               const std::string& eventId) const;

    Result<bool> filterEvent(const Event& event) const;

    Result<MentionBaseRef> fromEvent(const std::string& workspaceId, const Event& event,
                                     std::optional<ProjectObject> projectOpt) const;

    static std::string getKind() { return "mention.1"; };

private:
    std::shared_ptr<MentionApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync

#endif // MENTION_API_H
