#ifndef APSYNC_DATABASE_TYPE_H
#define APSYNC_DATABASE_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT DatabaseType {
    DatabaseType() : _type(file()) {}
    DatabaseType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const DatabaseType& other) const { return _type == other._type; }

    static DatabaseType file();
    static DatabaseType folder();
    static DatabaseType task();
    static DatabaseType project();

private:
    std::string _type;
};
} // namespace apsync

#endif // APSYNC_DATABASE_TYPE_H
