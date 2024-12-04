#ifndef JOURNAL_TYPE_H
#define JOURNAL_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT JournalType {
    JournalType() : _type(taskComment()) {}
    JournalType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const JournalType& other) const { return _type == other._type; }
    bool operator!=(const JournalType& other) const { return _type != other._type; }

    static JournalType taskComment();
    static JournalType taskChecked();
    static JournalType taskAttachment();
    static JournalType taskAttachmentDeleted();
    static JournalType historyTaskComment();
    static JournalType historyTaskChecked();
    static JournalType historyReviewAdded();
    static JournalType historyAttributesChanged();
    static JournalType historyTaskAttachment();
    static JournalType historyLocks();
    static JournalType historyCustom();

private:
    std::string _type;
};
} // namespace apsync

#endif // JOURNAL_TYPE_H
