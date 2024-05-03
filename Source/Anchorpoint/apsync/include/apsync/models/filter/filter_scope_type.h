#ifndef FILTER_SCOPE_TYPE_H
#define FILTER_SCOPE_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {

struct SYNC_EXPORT FilterScopeType {
    FilterScopeType() : _type() {}
    FilterScopeType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const FilterScopeType& other) const { return _type == other._type; }
    bool operator!=(const FilterScopeType& other) const { return _type != other._type; }

    static FilterScopeType all();
    static FilterScopeType atLeastOne();

private:
    std::string _type;
};
} // namespace apsync

#endif // FILTER_SCOPE_TYPE_H
