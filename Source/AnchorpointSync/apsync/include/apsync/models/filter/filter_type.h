#ifndef FILTER_TYPE_H
#define FILTER_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {

struct SYNC_EXPORT FilterType {

    FilterType() : _type() {}
    FilterType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const FilterType& other) const { return _type == other._type; }
    bool operator!=(const FilterType& other) const { return _type != other._type; }

    static FilterType andType();
    static FilterType orType();
    static FilterType singleTag();
    static FilterType multiTag();
    static FilterType text();
    static FilterType url();
    static FilterType rating();
    static FilterType member();
    static FilterType timeTracking();
    static FilterType fileSize();
    static FilterType date();
    static FilterType checkbox();

private:
    std::string _type;
};
} // namespace apsync
#endif // FILTER_TYPE_H
