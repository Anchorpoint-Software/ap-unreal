//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT TasksApi
{
public:
    explicit TasksApi(std::shared_ptr<Api> api);

    Result<void> listTasks(
        const Context& ctx,
        const std::string& path,
        const ListTasksOptions& options,
        const ListTasksDelegate& delegate
    );

    Result<TaskEntryWithAttributes> getTask(
        const std::string& path
    );

private:
    std::shared_ptr<class TasksApiImpl> _impl;
};

} // namespace apsync
//$generated