#ifndef INCLUDE_MODELS_OBJECT
#define INCLUDE_MODELS_OBJECT

#include <vector>

#include "apsync/internal/models/access_rule.h"
#include "apsync/sync_global.h"

namespace apsync {
struct Object {
    std::string kind;
    std::string props_data;

    std::optional<std::string> resource;
    std::optional<std::string> id;
    std::optional<long long> syncedAt;
    std::optional<long long> createdAt;
    std::optional<std::vector<AccessRule>> access;

    std::optional<std::string> projectId;

    std::optional<int64> revision;
};
} // namespace apsync

#endif /* INCLUDE_MODELS_OBJECT */
