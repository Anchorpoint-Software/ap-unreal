#ifndef ANCHORPOINT_BASE_OBJECT_H
#define ANCHORPOINT_BASE_OBJECT_H

#include "apsync/sync_global.h"

namespace apsync {
enum class ObjectType { FILE = 0, FOLDER, TASK, PROJECT, DATABASE, LOCK, UNKNOWN };

struct SYNC_EXPORT BaseObject {
    explicit BaseObject(ObjectType type = ObjectType::UNKNOWN) : type(type){};
    BaseObject(const std::string& id, const std::string& workspaceId, uint64 createdAt,
               ObjectType type, const std::string& resource = "")
        : id(id),
          workspaceId(workspaceId),
          createdAt(createdAt),
          resource(resource),
          type(type)
    {}
    std::string id;
    std::string workspaceId;
    uint64 createdAt;
    std::string resource;
    ObjectType type;
};
} // namespace apsync

#endif // ANCHORPOINT_BASE_OBJECT_H
