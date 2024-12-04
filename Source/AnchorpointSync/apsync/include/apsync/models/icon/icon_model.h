#ifndef ICON_MODEL_H
#define ICON_MODEL_H

#include <optional>
#include <string>

#include "apsync/sync_global.h"
#include "apsync/util/json.hpp"
#include "apsync/util/stringutils.h"

namespace apsync {
struct SYNC_EXPORT Icon {
    Icon() = default;
    explicit Icon(const std::string& path) { setIconPath(path); }

    Icon(const std::string& path, const std::optional<std::string>& color) : color(color)
    {
        setIconPath(path);
    }

    std::optional<std::string> path;
    std::optional<std::string> color;

    void update(const std::optional<std::string>& iconPath,
                const std::optional<std::string>& iconColor, bool reset);

    void setIconPath(const std::optional<std::string>& pathOpt)
    {
        if (pathOpt) {
            path = StringUtils::replace(*pathOpt, "qrc", "");
        } else {
            path = std::nullopt;
        }
    }

    bool operator==(const Icon& other) const { return path == other.path && color == other.color; }

    static std::optional<Icon> mapToIcon(const std::optional<std::string>& iconPath,
                                         const std::optional<std::string>& iconColor);
};


extern void from_json(const nlohmann::json& json, Icon& icon);
extern void to_json(nlohmann::json& json, const Icon& icon);

} // namespace apsync


#endif // ICON_MODEL_H
