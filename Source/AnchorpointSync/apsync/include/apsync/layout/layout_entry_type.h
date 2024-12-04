#ifndef LAYOUT_ENTRY_TYPE_H
#define LAYOUT_ENTRY_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT LayoutEntryType {
    LayoutEntryType() : _type(file()) {}
    LayoutEntryType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const LayoutEntryType& other) const { return _type == other._type; }
    bool operator==(const std::string& other) const { return _type == other; }

    static LayoutEntryType file();
    static LayoutEntryType folder();
    static LayoutEntryType task();
    static LayoutEntryType member();
    static LayoutEntryType recentFile();
    static LayoutEntryType project();
    static LayoutEntryType integration();

private:
    std::string _type;
};
} // namespace apsync

#endif // LAYOUT_ENTRY_TYPE_H
