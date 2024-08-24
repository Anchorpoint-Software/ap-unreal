#ifndef EVENT_MODEL_H
#define EVENT_MODEL_H

#include <vector>

#include "apsync/internal/models/access_rule.h"
#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT Event {
    std::string kind;
    std::string action;
    std::string data;

    std::optional<std::string> id;
    std::optional<std::string> subjectId;
    std::optional<std::string> sourceId;
    std::optional<uint64> createdAt;
    std::optional<uint64> syncedAt;
    std::optional<std::vector<AccessRule>> access;

    std::optional<std::string> projectId;
};
} // namespace apsync
#endif // EVENT_MODEL_H
