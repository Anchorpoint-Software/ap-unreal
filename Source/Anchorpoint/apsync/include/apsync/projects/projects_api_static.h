#ifndef PROJECTS_API_STATIC_H
#define PROJECTS_API_STATIC_H

#include "apsync/internal/service/project_file.h"
#include "apsync/sync_global.h"

namespace apsync {

class SYNC_EXPORT ProjectsApiStatic
{
public:
    static bool doesPathContainProject(const std::string& path, const std::string& projectId,
                                       const std::string& workspaceId);

    static bool doesPathContainProjectPathEntry(const std::string& path,
                                                const std::string& projectId,
                                                const std::string& workspaceId);

    static std::optional<ProjectFile> getProjectFileForPath(const std::string& path,
                                                            std::string* rootPath = nullptr);

    static bool doesPathContainAnyProject(const std::string& path);

    static bool isNullProjectId(const std::optional<std::string>& projectId);

    static std::string getNullProjectId();
};
} // namespace apsync

#endif // PROJECTS_API_STATIC_H
