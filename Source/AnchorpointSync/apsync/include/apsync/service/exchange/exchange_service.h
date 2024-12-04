#ifndef EXCHANGE_SERVICE_H
#define EXCHANGE_SERVICE_H

#include <vector>

#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class ExchangeServiceImpl;
class SYNC_EXPORT ExchangeService
{
public:
    explicit ExchangeService(std::shared_ptr<Api> api);

    Result<std::string> exportProject(
        const ProjectObject& project, const std::string& targetFolder,
        const std::optional<std::vector<std::string> >& userIds) const;
    Result<void> importProject(const std::string& exchangeFile, const std::string& targetWorkspace,
                               const std::string& targetProjectRoot) const;
    Result<ProjectObjectProps> getProjectMetadata(const std::string& exchangeFile) const;

private:
    std::shared_ptr<ExchangeServiceImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync

#endif // EXCHANGE_SERVICE_H
