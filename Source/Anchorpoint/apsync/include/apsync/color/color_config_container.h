#ifndef COLOR_CONFIG_CONTAINER_H
#define COLOR_CONFIG_CONTAINER_H

#include "apsync/util/json.hpp"
#include "models.h"

namespace apsync {

struct SYNC_EXPORT ColorConfigContainer {
    ColorConfig config;
    std::string hash;
    std::optional<std::string> localPath;
};

SYNC_EXPORT void to_json(nlohmann::json& json, const ColorConfig& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, ColorConfig& instance);

SYNC_EXPORT void to_json(nlohmann::json& json, const ColorConfigContainer& instance);
SYNC_EXPORT void from_json(const nlohmann::json& json, ColorConfigContainer& instance);
} // namespace apsync

#endif // COLOR_CONFIG_CONTAINER_H
