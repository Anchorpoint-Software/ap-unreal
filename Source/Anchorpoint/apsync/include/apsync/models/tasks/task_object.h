#ifndef TASK_OBJECT_H
#define TASK_OBJECT_H

#include "apsync/models/base_object.h"
#include "apsync/models/icon/icon_model.h"

namespace apsync {
struct SYNC_EXPORT TaskObjectProps {
    std::string name;
    bool checked = false;
    std::optional<std::string> checkedBy;
    std::optional<uint64> checkedAt;
    std::optional<Icon> icon;

    std::optional<std::string> projectId;
    int attachmentCount = 0;
};

struct SYNC_EXPORT TaskObject: public BaseObject {
    TaskObject() : BaseObject(ObjectType::TASK) {}
    // resource is databaseId
    TaskObjectProps properties;

    inline std::string getDatabaseId() const { return resource; }
};
} // namespace apsync

#endif // TASK_OBJECT_H
