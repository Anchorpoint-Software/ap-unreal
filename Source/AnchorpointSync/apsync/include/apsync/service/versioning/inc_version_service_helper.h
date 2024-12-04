#ifndef INC_VERSION_SERVICE_HELPER_H
#define INC_VERSION_SERVICE_HELPER_H

#include "apsync/sync_global.h"

namespace apsync {
class ObjectApi;
class ProjectsApi;
class ReviewsApi;
class AccountApi;
class Api;
class IncVersionService;
struct VersionedObject;
struct ProjectObject;
class SYNC_EXPORT IncVersionServiceHelperApi
{
public:
    IncVersionServiceHelperApi(std::shared_ptr<Api> api);

    Result<std::optional<VersionedObject>> getVersionedObject(std::string filePath);
    Result<std::optional<VersionedObject>> identifyVersionedObject(std::string filePath);

    Result<void> commentFile(std::string filePath, const std::optional<std::string>& comment);

    Result<std::string> getNextVersionName(std::string filePath);

    Result<std::string> createNextVersion(std::string filePath);

    Result<void> toggleVersionControl(std::string path, bool enable);

private:
    std::shared_ptr<Api> _api;
    std::shared_ptr<ObjectApi> _objectApi;
    std::shared_ptr<ProjectsApi> _projectsApi;
    std::shared_ptr<IncVersionService> _versionService;
    std::shared_ptr<ReviewsApi> _reviewsApi;
    std::shared_ptr<AccountApi> _accountApi;
};
} // namespace apsync

#endif // INC_VERSION_SERVICE_HELPER_H
