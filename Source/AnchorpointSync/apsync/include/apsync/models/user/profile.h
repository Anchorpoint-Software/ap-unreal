#ifndef USER_PROFILE_H
#define USER_PROFILE_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT UserProfile {
    std::optional<std::string> firstname;
    std::optional<std::string> lastname;
    std::optional<std::string> nickname;
    std::optional<std::string> picture;
};
} // namespace apsync
#endif // USER_PROFILE_H
