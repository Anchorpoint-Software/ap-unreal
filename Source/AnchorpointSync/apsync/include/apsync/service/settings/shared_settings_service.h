#ifndef SHARED_SETTINGS_SERVICE_H
#define SHARED_SETTINGS_SERVICE_H

#include "apsync/service/settings/base_settings_service.h"

namespace apsync {
class Api;
class SharedSettingsServiceImpl;
class SYNC_EXPORT SharedSettingsService: public BaseSettingsService
{
    friend class SharedSettingsServiceImpl;

public:
    SharedSettingsService(const SharedSettingsService& other);
    explicit SharedSettingsService(std::shared_ptr<Api> api);

    virtual Result<void> load(const std::string& workspaceId, const std::string& identifier);
    virtual Result<void> load(const std::string& projectId, const std::string& workspaceId,
                              const std::string& identifier);
    virtual Result<void> serialize() override;
    virtual Result<void> clear() override;

protected:
    std::shared_ptr<SharedSettingsServiceImpl> _pimpl;
};

using SharedSettings = SharedSettingsService;

} // namespace apsync

#endif // SHARED_SETTINGS_SERVICE_H
