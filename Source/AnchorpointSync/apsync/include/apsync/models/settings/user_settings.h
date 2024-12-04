#ifndef USER_SETTINGS_H
#define USER_SETTINGS_H

#include <unordered_set>

#include "realm_settings.h"

namespace apsync {
struct SYNC_EXPORT UserSettings {
    std::optional<std::string> userId;
    std::vector<RealmSettings> realms;
};
} // namespace apsync

#endif // USER_SETTINGS_H
