#ifndef BINDINGS_TASK_H
#define BINDINGS_TASK_H

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "apsync/models/icon/icon_model.h"

namespace python {
struct SYNC_EXPORT TaskBase {
    std::string workspaceId;
    std::optional<std::string> projectId;
};

struct SYNC_EXPORT TaskList: TaskBase {
    std::string name;
    std::string id;
};

struct SYNC_EXPORT Task: TaskBase {
    std::string name;
    std::string id;
    std::string listId;
    std::optional<apsync::Icon> icon;
};

using TaskListTarget = std::string;
class TaskApiImpl;
class SYNC_EXPORT TaskApi
{
public:
    explicit TaskApi(const std::string& workspaceId);
    void setWorkspaceId(const std::string& workspaceId);

    TaskList createTaskList(const TaskListTarget& target, const std::string& name);
    void renameTaskList(TaskList& list, const std::string& newName);
    void removeTaskList(const TaskList& list);
    std::vector<TaskList> getTaskLists(const TaskListTarget& target);
    std::optional<TaskList> getTaskList(const TaskListTarget& target, const std::string& name);
    TaskList getTaskListById(const std::string& id);
    Task createTask(const TaskList& list, const std::string& taskName);
    Task createTask(const TaskListTarget& target, const std::string& listName,
                    const std::string& taskName);

    std::optional<Task> getTask(const TaskList& list, const std::string& name);
    Task getTaskById(const std::string& taskId);

    std::vector<Task> getTasks(const TaskList& list);
    void renameTask(Task& task, const std::string& name);
    void removeTask(const Task& task);
    Task copyTask(const Task& task);
    void setTaskIcon(const Task& task, const std::optional<apsync::Icon>& icon);
    void setTaskIcons(const std::vector<Task>& tasks, const std::optional<apsync::Icon>& icon);

private:
    std::string _workspaceId;
    std::shared_ptr<TaskApiImpl> _impl;
};

} // namespace python

#endif // BINDINGS_TASK_H
