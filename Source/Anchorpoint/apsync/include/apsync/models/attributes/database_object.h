#ifndef ANCHORPOINT_DATABASE_OBJECT_H
#define ANCHORPOINT_DATABASE_OBJECT_H

#include "apsync/models/base_object.h"
#include "database_type.h"

namespace apsync {

struct SYNC_EXPORT DatabaseObjectProps {
    std::string name;
    DatabaseType type;
    std::string activeViewId;
    std::optional<std::string> projectId;
};

struct SYNC_EXPORT DatabaseObject: public BaseObject {
    DatabaseObject() : BaseObject(ObjectType::DATABASE) {}
    // resource is objectId of parent object
    DatabaseObjectProps properties;
    inline std::string getParentObjectId() const { return resource; }
};
} // namespace apsync

#endif // ANCHORPOINT_DATABASE_OBJECT_H
