#ifndef ACTION_SETTINGS_H
#define ACTION_SETTINGS_H

#include <unordered_map>
#include <vector>

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT ActionRepository {
    std::string name;
    std::string url;
    std::optional<std::string> shaId;
};

struct SYNC_EXPORT ActionSettings {
    std::vector<ActionRepository> repos;

    // maps action id / package id to enabled state
    std::unordered_map<std::string, bool> enabledActionsOverwrite;
};
} // namespace apsync

#endif // ACTION_SETTINGS_H
