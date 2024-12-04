#ifndef DATABASE_VIEW_TYPE_H
#define DATABASE_VIEW_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT DatabaseViewType {
    DatabaseViewType() : _type(table()) {}
    DatabaseViewType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const DatabaseViewType& other) const { return _type == other._type; }
    bool operator!=(const DatabaseViewType& other) const { return _type != other._type; }

    static DatabaseViewType table();
    static DatabaseViewType grid();

private:
    std::string _type;
};
} // namespace apsync

#endif // DATABASE_VIEW_TYPE_H
