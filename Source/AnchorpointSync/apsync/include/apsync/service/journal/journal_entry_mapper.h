#ifndef JOURNAL_ENTRY_MAPPER_H
#define JOURNAL_ENTRY_MAPPER_H

#include "apsync/internal/models/document.h"
#include "apsync/models/journal/journal_entry.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {

class SYNC_EXPORT JournalEntryMapper
{
public:
    explicit JournalEntryMapper();

    static Result<InternalJournalEntry> unmarshalJournalEntry(const Document& doc,
                                                              const std::string& objectId,
                                                              const std::string& workspaceId);
    static std::string marshalJournalEntryContent(const JournalEntryContent& content);

    static Result<InternalJournalEntry> map(const BaseRequestAddJournalEntryRef& request);
    static Result<InternalJournalEntry> map(const BaseJournalEntryRef& entry);

    static Result<InternalJournalEntry> mapToHistoryEntry(const InternalJournalEntry& entry);

    struct InternalJournalEntryPatchResult {
        std::optional<InternalJournalEntry> entry = std::nullopt;
        enum class Operation { SKIP, UPDATE, REMOVE } op = Operation::SKIP;
    };

    static InternalJournalEntryPatchResult patchEntry(const InternalJournalEntry& newEntry,
                                                      const InternalJournalEntry& existingEntry);
};
} // namespace apsync

#endif // JOURNAL_ENTRY_MAPPER_H
