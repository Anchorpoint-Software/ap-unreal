#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "task_object.h"

namespace apsync {
struct SYNC_EXPORT TaskList {
    std::string id;
    std::string name;
    std::vector<TaskObject> tasks;
};
} // namespace apsync

#endif // TASK_LIST_H
