#ifndef FILTER_OPERATION_TYPE_H
#define FILTER_OPERATION_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {

struct SYNC_EXPORT FilterOperationType {

    FilterOperationType() : _type() {}
    FilterOperationType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const FilterOperationType& other) const { return _type == other._type; }
    bool operator!=(const FilterOperationType& other) const { return _type != other._type; }

    static FilterOperationType contains();
    static FilterOperationType notContains();
    static FilterOperationType equals();
    static FilterOperationType doesNotEqual();
    static FilterOperationType isEmpty();
    static FilterOperationType isNotEmpty();
    static FilterOperationType startsWith();
    static FilterOperationType endsWith();
    static FilterOperationType equalsRegex();

    static FilterOperationType greaterThan();
    static FilterOperationType lessThan();
    static FilterOperationType greaterThanOrEqual();
    static FilterOperationType lessThanOrEqual();
    static FilterOperationType zero();
    static FilterOperationType notZero();
    static FilterOperationType between();
    static FilterOperationType min();
    static FilterOperationType max();

    static FilterOperationType before();
    static FilterOperationType after();
    static FilterOperationType today();
    static FilterOperationType thisWeek();
    static FilterOperationType thisMonth();
    static FilterOperationType yesterday();
    static FilterOperationType lastWeek();
    static FilterOperationType lastMonth();
    static FilterOperationType tomorrow();
    static FilterOperationType nextWeek();
    static FilterOperationType nextMonth();

private:
    std::string _type;
};

} // namespace apsync

#endif // FILTER_OPERATION_TYPE_H
