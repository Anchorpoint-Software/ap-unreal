#ifndef JOURNAL_ENTRY_CREATOR_H
#define JOURNAL_ENTRY_CREATOR_H

#include <chrono>
#include <set>

#include "apsync/models/journal/journal_entry.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {

class Api;
class JournalEntryCreatorImpl;
class SYNC_EXPORT JournalEntryCreator
{
public:
    explicit JournalEntryCreator(std::shared_ptr<Api> api);

    Result<std::optional<InternalJournalEntry>> addJournalEntry(
        const std::string& objectId, const std::string& workspaceId,
        const BaseRequestAddJournalEntryRef& request);

    Result<std::optional<InternalJournalEntry>> addJournalEntry(
        const std::string& objectId, const std::string& workspaceId,
        const InternalJournalEntry& internalEntry,
        PATCH_INTERVAL patchInterval = PATCH_INTERVAL::DEFAULT);

    Result<void> updateJournalEntry(const std::string& objectId, const std::string& workspaceId,
                                    const std::string& entryId,
                                    PATCH_INTERVAL patchInterval = PATCH_INTERVAL::DEFAULT);

    Result<void> deleteJournalEntry(const std::string& objectId, const std::string& workspaceId,
                                    const std::string& entryId);

private:
    Result<std::multiset<InternalJournalEntry, std::less<>>> getJournalUntil(
        const std::string& objectId, const std::string& workspaceId,
        std::chrono::seconds timeOffset) const;

    std::shared_ptr<JournalEntryCreatorImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync

#endif // JOURNAL_ENTRY_CREATOR_H
