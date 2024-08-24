#ifndef FILESTACK_HELPER_H
#define FILESTACK_HELPER_H

#include "apsync/models/versioning/versioned_object.h"
#include "apsync/sync_global.h"

namespace apsync {
class ObjectApi;
class ProjectsApi;
class IncVersionService;
class SYNC_EXPORT FileStackHelperApi
{
public:
    FileStackHelperApi(std::shared_ptr<ObjectApi> objectApi,
                       std::shared_ptr<ProjectsApi> projectsApi,
                       std::shared_ptr<IncVersionService> versionService);

    Result<std::string> resolveFilestackBasefile(const std::string& filePath, bool& isFilestack,
                                                 IncrementStyle& style) const;

    static bool suffixIsSequenceCandidate(const std::string& filePath);

private:
    std::shared_ptr<ObjectApi> _objectApi;
    std::shared_ptr<ProjectsApi> _projectsApi;
    std::shared_ptr<IncVersionService> _versionService;
};
} // namespace apsync

#endif // FILESTACK_HELPER_H
