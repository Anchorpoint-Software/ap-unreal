#ifndef API_VERSION_H
#define API_VERSION_H

#include "apsync/sync_global.h"
#include "apsync/version.h"
#include "stringutils.h"

namespace apsync {
class SYNC_EXPORT ApiVersion
{
public:
    ApiVersion(int major, int minor, int patch);

    ApiVersion(const std::string& version);

    static ApiVersion currentVersion() { return ApiVersion(AP_MAJOR, AP_MINOR, AP_PATCH); }

    std::string toString() const;
    int getMajor() const;
    int getMinor() const;
    int getPatch() const;

    bool operator==(const ApiVersion& other) const;
    bool operator!=(const ApiVersion& other) const;
    bool operator>(const ApiVersion& other) const;
    bool operator<(const ApiVersion& other) const;
    bool operator>=(const ApiVersion& other) const;
    bool operator<=(const ApiVersion& other) const;

private:
    int major, minor, patch;
};
} // namespace apsync
#endif // API_VERSION_H
