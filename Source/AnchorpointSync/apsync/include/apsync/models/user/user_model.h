#ifndef USER_MODEL_H
#define USER_MODEL_H

#include "apsync/sync_global.h"
#include "profile.h"

namespace apsync {
struct SYNC_EXPORT UserModel {
    std::string id;
    std::string authId;
    std::string userspaceId;
    std::string email;
    UserProfile internalProfile;
    UserProfile externalProfile;
};
} // namespace apsync

#endif // USER_MODEL_H
