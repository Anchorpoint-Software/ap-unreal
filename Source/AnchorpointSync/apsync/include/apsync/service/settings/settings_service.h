#ifndef SETTINGS_SERVICE_H
#define SETTINGS_SERVICE_H

#include "apsync/service/settings/base_settings_service.h"

namespace apsync {
class Api;
class SettingsServiceImpl;
class SYNC_EXPORT SettingsService: public BaseSettingsService
{
    friend class SettingsServiceImpl;

public:
    explicit SettingsService(std::shared_ptr<Api> api);
    SettingsService(const SettingsService& other);

    virtual Result<void> load(const std::optional<std::string>& name,
                              const std::optional<std::string>& identifier, bool userOnly = true,
                              const std::optional<std::string>& settingsLocation = std::nullopt);
    virtual Result<void> serialize() override;

protected:
    std::shared_ptr<SettingsServiceImpl> _pimpl;
};
} // namespace apsync
#endif // SETTINGS_SERVICE_H
