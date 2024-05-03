#ifndef ANCHORPOINT_BASE_DOCUMENT_H
#define ANCHORPOINT_BASE_DOCUMENT_H

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT BaseDocument {
    BaseDocument() = default;
    BaseDocument(const std::string& id, const std::string& workspaceId, uint64 createdAt)
        : id(id),
          workspaceId(workspaceId),
          createdAt(createdAt)
    {}
    std::string id;
    std::string workspaceId;
    uint64 createdAt;
};
} // namespace apsync

#endif // ANCHORPOINT_BASE_DOCUMENT_H
