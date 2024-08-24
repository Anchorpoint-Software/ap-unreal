#ifndef INCLUDE_MODELS_DOCUMENT
#define INCLUDE_MODELS_DOCUMENT

#include <vector>

#include "apsync//internal/models/access_rule.h"
#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT Document {
    std::string objectId;
    std::string contentData;

    std::optional<std::string> id;
    std::optional<int64> syncedAt;
    std::optional<int64> createdAt;
    std::optional<std::vector<AccessRule>> access;

    std::optional<int64> revision;
};
} // namespace apsync

#endif /* INCLUDE_MODELS_DOCUMENT */
