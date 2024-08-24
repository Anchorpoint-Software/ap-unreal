#ifndef LOCK_OBJECT_H
#define LOCK_OBJECT_H

#include "apsync/models/base_object.h"
#include "apsync/users/models.h"

namespace apsync {

struct SYNC_EXPORT LockObjectProps {
    ObjectType type;
    unordered_map<string, string> data;
    optional<string> description;
};

struct SYNC_EXPORT LockObject: public BaseObject {
    LockObject() : BaseObject(ObjectType::LOCK){};

    LockObjectProps properties;
    string getTargetId() const { return resource; }

    string projectId;
    string userId;
    optional<string> path;
};

struct SYNC_EXPORT LockObjectChange {
    string targetId;
    string ownerId;
    LockObjectProps props;
};
} // namespace apsync

#endif // LOCK_OBJECT_H
