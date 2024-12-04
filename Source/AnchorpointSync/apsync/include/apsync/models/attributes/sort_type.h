#ifndef SORT_TYPE_H
#define SORT_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {

struct SYNC_EXPORT SortType {
    SortType() : _type(ascending()) {}
    SortType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const SortType& other) const { return _type == other._type; }
    bool operator!=(const SortType& other) const { return _type != other._type; }

    static SortType ascending();
    static SortType descending();
    static SortType custom();

private:
    std::string _type;
};

} // namespace apsync

#endif // SORT_TYPE_H
