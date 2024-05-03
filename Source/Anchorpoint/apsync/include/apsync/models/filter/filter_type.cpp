#include "filter_type.h"

apsync::FilterType apsync::FilterType::andType()
{
    static const FilterType f = FilterType("and");
    return f;
}

apsync::FilterType apsync::FilterType::orType()
{
    static const FilterType f = FilterType("or");
    return f;
}

apsync::FilterType apsync::FilterType::singleTag()
{
    static const FilterType f = FilterType("singleTag");
    return f;
}

apsync::FilterType apsync::FilterType::multiTag()
{
    static const FilterType f = FilterType("multiTag");
    return f;
}

apsync::FilterType apsync::FilterType::text()
{
    static const FilterType f = FilterType("text");
    return f;
}

apsync::FilterType apsync::FilterType::url()
{
    static const FilterType f = FilterType("url");
    return f;
}

apsync::FilterType apsync::FilterType::rating()
{
    static const FilterType f = FilterType("rating");
    return f;
}

apsync::FilterType apsync::FilterType::member()
{
    static const FilterType f = FilterType("member");
    return f;
}

apsync::FilterType apsync::FilterType::timeTracking()
{
    static const FilterType f = FilterType("timeTracking");
    return f;
}

apsync::FilterType apsync::FilterType::fileSize()
{
    static const FilterType f = FilterType("fileSize");
    return f;
}

apsync::FilterType apsync::FilterType::date()
{
    static const FilterType f = FilterType("date");
    return f;
}

apsync::FilterType apsync::FilterType::checkbox()
{
    static const FilterType f = FilterType("checkbox");
    return f;
}
