#ifndef NAMED_SETTINGS_H
#define NAMED_SETTINGS_H

#include <optional>
#include <unordered_map>

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT SettingsValue {
    std::optional<bool> boolean;
    std::optional<std::string> string;
    std::optional<float> decimal;
    std::optional<int> number;
    std::optional<std::vector<std::string>> list;
};

struct SYNC_EXPORT NamedSettings {
    NamedSettings() = default;
    NamedSettings(const std::string& name) : name(name) {}
    std::string name;

    std::unordered_map<std::string, SettingsValue> settings;

    bool operator==(const NamedSettings& other) const { return name == other.name; }
};
} // namespace apsync

namespace std {
template<>
struct hash<apsync::NamedSettings> {
    std::size_t operator()(const apsync::NamedSettings& settings) const
    {
        return hash<std::string>()(settings.name);
    }
};
} // namespace std

#endif // NAMED_SETTINGS_H
