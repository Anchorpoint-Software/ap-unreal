#ifndef JOURNAL_ENTRY_RESOLVER_H
#define JOURNAL_ENTRY_RESOLVER_H

#include "apsync/models/journal/journal_entry.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {

class Api;
class JournalEntryResolverImpl;
class SYNC_EXPORT JournalEntryResolver
{
public:
    explicit JournalEntryResolver(std::shared_ptr<Api> api);

    Result<BaseJournalEntryRef> resolve(const std::optional<apsync::ProjectObject>& projectObject,
                                        const InternalJournalEntry& internalEntry);

private:
    std::shared_ptr<JournalEntryResolverImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync

#endif // JOURNAL_ENTRY_RESOLVER_H
