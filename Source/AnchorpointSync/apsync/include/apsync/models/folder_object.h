#ifndef ANCHORPOINT_FOLDER_OBJECT_H
#define ANCHORPOINT_FOLDER_OBJECT_H

#include "apsync/error/error.h"
#include "apsync/models/icon/icon_model.h"
#include "apsync/util/filesystem.hpp"
#include "apsync/util/stringutils.h"
#include "base_object.h"
#include "project_object.h"

namespace apsync {
struct SYNC_EXPORT FolderObjectProps {
    std::optional<std::string> projectId;
    bool vcEnabled = false;
    bool sequenceStackEnabled = false;
    std::optional<Icon> icon;

    bool operator==(const FolderObjectProps& other)
    {
        return projectId == other.projectId && vcEnabled == other.vcEnabled
            && sequenceStackEnabled == other.sequenceStackEnabled && icon == other.icon;
    }
    bool operator!=(const FolderObjectProps& other) { return !(*this == other); }
};

class ObjectApi;
struct SYNC_EXPORT FolderObject: public BaseObject {
    // resource is path relative to project root / absolute path w/o project
    friend class ObjectApi;
    FolderObject() : BaseObject(ObjectType::FOLDER) {}

    FolderObjectProps properties;

    // Returns the absolute path of the folder.
    inline Result<std::string> getFolderpath(
        const std::optional<ProjectObject>& projectOpt = std::nullopt) const
    {
        if (absolutePath) return *absolutePath;

        if (projectOpt) {
            if (!projectOpt->rootPath || projectOpt->rootPath->empty()) {
                return SyncError::InvalidObject;
            }
            if (resource == ".") {
                return *projectOpt->rootPath;
            } else {
                if (ghc::filesystem::path(resource).is_absolute()) { return resource; }
            }

            if (StringUtils::endsWith(*projectOpt->rootPath, "/")) {
                return *projectOpt->rootPath + resource;
            }
            return *projectOpt->rootPath + "/" + resource;
        }
        return resource;
    }

    void setAbsolutePath(const std::string& path) { absolutePath = path; }

private:
    std::optional<std::string> absolutePath;
};
} // namespace apsync

#endif // ANCHORPOINT_FOLDER_OBJECT_H
