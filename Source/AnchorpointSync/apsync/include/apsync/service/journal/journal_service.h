#ifndef JOURNAL_SERVICE_H
#define JOURNAL_SERVICE_H

#include <chrono>
#include <set>
#include <unordered_set>
#include <vector>

#include "apsync/models/journal/journal_entry.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"


namespace apsync {

class Api;
class JournalServiceImpl;

class SYNC_EXPORT JournalService
{
public:
    explicit JournalService(std::shared_ptr<Api> api);

    Result<std::vector<BaseJournalEntryRef>> getJournal(
        const std::optional<apsync::ProjectObject>& projectObject, const std::string& objectId,
        const std::string& workspaceId) const;

    Result<std::vector<BaseJournalEntryRef>> getJournalFiltered(
        const std::optional<apsync::ProjectObject>& projectObject,
        const std::unordered_set<std::string>& filterEntryIds, const std::string& objectId,
        const std::string& workspaceId) const;

    Result<std::vector<BaseJournalEntryRef>> getJournalRange(
        const std::optional<apsync::ProjectObject>& projectObject,
        const std::optional<std::string>& startId, const std::string& endId,
        const std::string& objectId, const std::string& workspaceId) const;

    Result<JournalPage> getJournalPage(const std::optional<apsync::ProjectObject>& projectObject,
                                       uint64 startTimestampInSec, int pageSize,
                                       const std::string& objectId,
                                       const std::string& workspaceId) const;

    Result<JournalPage> getJournalPage(const std::optional<apsync::ProjectObject>& projectObject,
                                       uint64 startTimestampInSec, std::chrono::seconds timeRange,
                                       const std::string& objectId,
                                       const std::string& workspaceId) const;

    Result<std::optional<BaseJournalEntryRef>> getNewestJournalEntry(
        const std::optional<apsync::ProjectObject>& projectObject, const std::string& objectId,
        const std::string& workspaceId);

    Result<BaseJournalEntryRef> addJournalEntry(const std::optional<ProjectObject>& projectObject,
                                                const std::string& objectId,
                                                const std::string& workspaceId,
                                                const BaseRequestAddJournalEntryRef& request);

    Result<BaseJournalEntryRef> updateJournalEntry(
        const std::optional<ProjectObject>& projectObject, const BaseJournalEntryRef& entry);

    Result<void> deleteJournalEntry(const std::optional<ProjectObject>& projectObject,
                                    const std::string& objectId, const std::string& workspaceId,
                                    const std::string& entryId);

    Result<void> sync(const std::string& objectId, const std::string& workspaceId);

private:
    Result<std::multiset<InternalJournalEntry, std::less<>>> getJournalInternal(
        const std::string& objectId, const std::string& workspaceId) const;

    std::shared_ptr<JournalServiceImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync

#endif // JOURNAL_SERVICE_H
