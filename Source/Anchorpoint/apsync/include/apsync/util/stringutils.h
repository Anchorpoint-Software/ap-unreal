#ifndef INCLUDE_UTIL_STRINGUTILS
#define INCLUDE_UTIL_STRINGUTILS

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "apsync/sync_global.h"

namespace apsync {
class SYNC_EXPORT StringUtils
{
public:
    static bool startsWith(const string& s, const string& prefix, bool caseInsensitive = false);

    static bool endsWith(const string& s, const string& suffix, bool caseInsensitive = false);

    static bool contains(const string& s, const string& part, bool caseInsensitive = false);

    static bool equals(const string& a, const string& b, bool caseInsensitive = false);

    static string::size_type find(const string& s, const string& partIn,
                                  bool caseInsensitive = false);

    static int countCharOccurences(string str, char ch);

    static int compare(const string& a, const string& b);

    static void normalizePath(string& path);

    static string normalizeDrive(const string& str);

    static string normalizeEncoding(const string& str);

    // Source:
    // https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
    template<typename... Args>
    static string stringFormat(const string& format, Args... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
        if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, format.c_str(), args...);
        return {buf.get(), buf.get() + size - 1}; // We don't want the '\0' inside
    }

    static string getAppDataPath();
    static string getAnchorpointDataPath();
    static string getApplicationRoot();

    static string replaceAll(const string& value, const string& source, const string& replace);

    static string replace(const string& value, const string& source, const string& replace);

    static vector<string> split(const string& value, char delimiter);

    static string trim(const string& value);

    static string lowercase(const string& s);

    static string escapeJsonString(const string& jsonS);
};
} // namespace apsync
#endif /* INCLUDE_UTIL_STRINGUTILS */
