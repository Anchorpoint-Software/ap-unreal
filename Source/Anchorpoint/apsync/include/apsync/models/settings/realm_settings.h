#ifndef SETTING_REALM_H
#define SETTING_REALM_H

#include <unordered_set>
#include <vector>

#include "named_settings.h"

namespace apsync {
struct SYNC_EXPORT RealmSettings {
    RealmSettings() = default;
    RealmSettings(const std::string& id) : id(id) {}
    std::string id;
    std::vector<NamedSettings> namedSettings;

    bool operator==(const RealmSettings& other) const { return id == other.id; }
    bool operator==(const std::string& identifier) const { return id == identifier; }
};
} // namespace apsync

namespace std {
template<>
struct hash<apsync::RealmSettings> {
    std::size_t operator()(const apsync::RealmSettings& realm) const
    {
        return hash<std::string>()(realm.id);
    }
};
} // namespace std

#endif // SETTING_REALM_H
