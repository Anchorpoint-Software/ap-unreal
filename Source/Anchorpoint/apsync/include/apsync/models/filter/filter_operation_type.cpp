#include "filter_operation_type.h"

apsync::FilterOperationType apsync::FilterOperationType::contains()
{
    static const FilterOperationType f = FilterOperationType("contains");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::notContains()
{
    static const FilterOperationType f = FilterOperationType("notContains");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::equals()
{
    static const FilterOperationType f = FilterOperationType("equals");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::doesNotEqual()
{
    static const FilterOperationType f = FilterOperationType("doesNotEqual");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::isEmpty()
{
    static const FilterOperationType f = FilterOperationType("isEmpty");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::isNotEmpty()
{
    static const FilterOperationType f = FilterOperationType("isNotEmpty");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::startsWith()
{
    static const FilterOperationType f = FilterOperationType("startsWith");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::endsWith()
{
    static const FilterOperationType f = FilterOperationType("endsWith");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::equalsRegex()
{
    static const FilterOperationType f = FilterOperationType("equalsRegex");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::greaterThan()
{
    static const FilterOperationType f = FilterOperationType("greaterThan");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::lessThan()
{
    static const FilterOperationType f = FilterOperationType("lessThan");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::greaterThanOrEqual()
{
    static const FilterOperationType f = FilterOperationType("greaterThanOrEqual");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::lessThanOrEqual()
{
    static const FilterOperationType f = FilterOperationType("lessThanOrEqual");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::zero()
{
    static const FilterOperationType f = FilterOperationType("zero");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::notZero()
{
    static const FilterOperationType f = FilterOperationType("notZero");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::between()
{
    static const FilterOperationType f = FilterOperationType("between");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::min()
{
    static const FilterOperationType f = FilterOperationType("min");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::max()
{
    static const FilterOperationType f = FilterOperationType("max");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::before()
{
    static const FilterOperationType f = FilterOperationType("before");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::after()
{
    static const FilterOperationType f = FilterOperationType("after");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::today()
{
    static const FilterOperationType f = FilterOperationType("today");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::thisWeek()
{
    static const FilterOperationType f = FilterOperationType("thisWeek");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::thisMonth()
{
    static const FilterOperationType f = FilterOperationType("thisMonth");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::yesterday()
{
    static const FilterOperationType f = FilterOperationType("yesterday");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::lastWeek()
{
    static const FilterOperationType f = FilterOperationType("lastWeek");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::lastMonth()
{
    static const FilterOperationType f = FilterOperationType("lastMonth");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::tomorrow()
{
    static const FilterOperationType f = FilterOperationType("tomorrow");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::nextWeek()
{
    static const FilterOperationType f = FilterOperationType("nextWeek");
    return f;
}

apsync::FilterOperationType apsync::FilterOperationType::nextMonth()
{
    static const FilterOperationType f = FilterOperationType("nextMonth");
    return f;
}
