#include "filter_scope_type.h"

apsync::FilterScopeType apsync::FilterScopeType::all()
{
    static const FilterScopeType s = FilterScopeType("all");
    return s;
}

apsync::FilterScopeType apsync::FilterScopeType::atLeastOne()
{
    static const FilterScopeType s = FilterScopeType("atLeastOne");
    return s;
}
