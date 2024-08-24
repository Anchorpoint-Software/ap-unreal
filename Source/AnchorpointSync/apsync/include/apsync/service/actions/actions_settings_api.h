#ifndef ACTIONS_SETTINGS_API_H
#define ACTIONS_SETTINGS_API_H

#include "apsync/account/account_api.h"
#include "apsync/models/actions/action_settings.h"
#include "apsync/util/lazy.h"

namespace apsync {

class Api;
class ActionsSettingsApiImpl;
class SYNC_EXPORT ActionsSettingsApi
{
public:
    explicit ActionsSettingsApi(std::shared_ptr<Api> api);

    Result<ActionSettings> getOrCreateSettings(const std::string& workspaceId);
    Result<std::optional<ActionSettings>> getSettings(const std::string& workspaceId) const;
    Result<void> setSettings(const std::string& workspaceId, const ActionSettings& settings) const;

private:
    std::shared_ptr<ActionsSettingsApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
    Lazy<AccountApi> _accountApi;
};
} // namespace apsync

#endif // ACTIONS_SETTINGS_API_H
