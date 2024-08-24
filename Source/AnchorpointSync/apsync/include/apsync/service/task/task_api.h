#ifndef TASK_API_H
#define TASK_API_H

#include <vector>

#include "apsync/models/attachment/attachment.h"
#include "apsync/models/attributes/database_object.h"
#include "apsync/models/file_object.h"
#include "apsync/models/project_object.h"
#include "apsync/models/tasks/task_object.h"
#include "apsync/sync_global.h"

namespace apsync {

class Api;
class TaskApiImpl;
struct BaseAttachment;
using AttachmentRef = std::shared_ptr<BaseAttachment>;
class SYNC_EXPORT TaskApi
{
public:
    explicit TaskApi(std::shared_ptr<Api> api);

    Result<DatabaseObject> createTaskDatabase(const std::string& name,
                                              const BaseObject& parentObject,
                                              const std::optional<std::string>& projectId);

    Result<std::optional<DatabaseObject>> getTaskDatabase(const std::string& name,
                                                          const BaseObject& parentObject) const;
    Result<DatabaseObject> getTaskDatabase(const std::string& databaseId,
                                           const std::string& workspaceId) const;
    Result<std::vector<DatabaseObject>> getTaskDatabases(const BaseObject& parentObject) const;

    Result<std::vector<TaskObject>> getTasks(const DatabaseObject& database) const;
    Result<std::vector<TaskObject>> getTasks(const std::string& databaseId,
                                             const std::string& workspaceId) const;
    Result<std::vector<TaskObject>> getTasksForFile(FileObject& file) const;
    Result<TaskObject> getTask(const std::string& taskId, const std::string& workspaceId) const;
    Result<std::vector<TaskObject>> getTasks(const std::vector<std::string>& taskIds,
                                             const std::string& workspaceId) const;

    Result<TaskObject> addTask(const DatabaseObject& database, const std::string& name);
    Result<TaskObject> addTask(const DatabaseObject& database, const TaskObjectProps& properties);

    Result<void> deleteTask(const TaskObject& task);
    Result<void> deleteTask(const std::string& taskId, const std::string& workspaceId);
    Result<void> deleteTasks(const std::vector<std::string>& taskIds,
                             const std::string& workspaceId);

    Result<void> deleteTaskDatabase(const DatabaseObject& database);
    Result<void> deleteTaskDatabase(const std::string& databaseId, const std::string& workspaceId);

    Result<TaskObject> copyTask(const DatabaseObject& database, const TaskObject& task);
    Result<TaskObject> moveTask(const DatabaseObject& dstDatabase, const TaskObject& task);
    Result<std::unordered_map<std::string, std::string>> moveTasks(
        const DatabaseObject& srcDatabase, const DatabaseObject& dstDatabase,
        const std::vector<std::string>& taskIds);
    Result<std::unordered_map<std::string, std::string>> copyTasks(
        const DatabaseObject& srcDatabase, const DatabaseObject& dstDatabase,
        const std::vector<std::string>& taskIds);

    Result<void> setName(TaskObject& task, const std::string& name);

    Result<void> updateIcons(const std::vector<std::string>& taskIds,
                             const std::optional<std::string>& iconPath,
                             const std::optional<std::string>& iconColor,
                             const std::string& workspaceId);
    Result<void> setIcons(const std::vector<std::string>& taskIds, const std::optional<Icon>& icon,
                          const std::string& workspaceId);

    Result<void> setChecked(const std::optional<apsync::ProjectObject>& project, TaskObject& task,
                            bool checked, const std::optional<std::string>& checkedBy,
                            std::optional<uint64> checkedAt);

    Result<void> attach(const std::optional<ProjectObject>& projectObject, TaskObject& task,
                        const std::string& userId, const std::vector<std::string>& paths,
                        bool forceLocalToProject = false);

    Result<void> promoteExternalAttachments(const std::optional<ProjectObject>& projectObject,
                                            const TaskObject& task,
                                            const std::vector<std::string> attachmentIds,
                                            const std::string& taskFolderPath,
                                            const std::string& targetFolder);

    Result<std::vector<AttachmentRef>> getAttachments(
        const std::optional<ProjectObject>& projectObject, const TaskObject& task,
        bool resolveSubEntries) const;

    Result<void> deleteAttachments(TaskObject& task,
                                   const std::vector<std::string> attachmentIds) const;
    Result<void> deleteAllAttachments(TaskObject& task) const;

    Result<void> toggleFavoriteOnAttachment(const std::string& attachmentId,
                                            const std::string& taskId,
                                            const std::string& workspaceId,
                                            const std::string& userId, bool favorited) const;

private:
    std::shared_ptr<TaskApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync
#endif // TASK_API_H
