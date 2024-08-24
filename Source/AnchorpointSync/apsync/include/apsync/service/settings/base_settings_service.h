#ifndef BASE_SETTINGS_SERVICE_H
#define BASE_SETTINGS_SERVICE_H

#include "apsync/models/settings/named_settings.h"
#include "apsync/models/settings/user_settings.h"
#include "apsync/sync_global.h"
#include "apsync/util/json.hpp"

namespace apsync {

extern void to_json(nlohmann::json& nlohmann_json_j, const NamedSettings& nlohmann_json_t);
extern void from_json(const nlohmann::json& nlohmann_json_j, NamedSettings& nlohmann_json_t);
extern void to_json(nlohmann::json& nlohmann_json_j, const RealmSettings& nlohmann_json_t);
extern void from_json(const nlohmann::json& nlohmann_json_j, RealmSettings& nlohmann_json_t);
extern void to_json(nlohmann::json& nlohmann_json_j, const UserSettings& nlohmann_json_t);
extern void from_json(const nlohmann::json& nlohmann_json_j, UserSettings& nlohmann_json_t);
extern void to_json(nlohmann::json& nlohmann_json_j, const SettingsValue& nlohmann_json_t);
extern void from_json(const nlohmann::json& nlohmann_json_j, SettingsValue& nlohmann_json_t);

class Api;
class AccountApi;

class SYNC_EXPORT BaseSettingsService
{
public:
    explicit BaseSettingsService(std::shared_ptr<Api> api);
    BaseSettingsService(const BaseSettingsService& other);

    virtual Result<void> serialize() = 0;
    virtual Result<void> clear();

    std::optional<std::string> getString(const std::string& key) const;
    std::optional<std::vector<std::string>> getList(const std::string& key) const;
    std::optional<int> getInt(const std::string& key) const;
    std::optional<float> getFloat(const std::string& key) const;
    std::optional<bool> getBool(const std::string& key) const;
    std::optional<SettingsValue> get(const std::string& key) const;

    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, float value);
    void set(const std::string& key, bool value);
    void set(const std::string& key, const std::vector<std::string>& value);

    bool contains(const std::string& key) const;

    void remove(const std::string& key);

protected:
    std::optional<NamedSettings> _settings;
    std::optional<std::string> _userId;
    std::optional<std::string> _realmId;
    bool _userOnly;


    std::shared_ptr<Api> _api;
    std::shared_ptr<AccountApi> _accountApi;
};
} // namespace apsync

#endif // BASE_SETTINGS_SERVICE_H
