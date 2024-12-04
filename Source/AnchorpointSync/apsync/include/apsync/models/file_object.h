#ifndef ANCHORPOINT_FILE_OBJECT_H
#define ANCHORPOINT_FILE_OBJECT_H

#include <unordered_set>

#include "apsync/models/icon/icon_model.h"
#include "apsync/util/algorithm.h"
#include "apsync/util/filesystem.hpp"
#include "apsync/util/stringutils.h"
#include "base_object.h"
#include "folder_object.h"

namespace apsync {
struct SYNC_EXPORT FileObjectProps {
    std::optional<std::string> projectId;
    std::optional<std::string> previewRes;
    std::optional<std::string> detailRes;
    std::optional<std::string> versionId;
    std::optional<std::unordered_set<std::string>> attachedToTaskIds; // attached to tasks
    std::optional<Icon> icon;
    int reviewCount = 0;

    bool operator==(const FileObjectProps& other) const
    {
        return projectId == other.projectId && previewRes == other.previewRes
            && detailRes == other.detailRes && reviewCount == other.reviewCount
            && versionId == other.versionId
            && compareOptionalContainer(attachedToTaskIds, other.attachedToTaskIds)
            && icon == other.icon;
    }
    bool operator!=(const FileObjectProps& other) const { return !(*this == other); }
};

class ObjectApi;
struct SYNC_EXPORT FileObject: public BaseObject {
    friend class ObjectApi;
    FileObject() : BaseObject(ObjectType::FILE) {}

    FileObjectProps properties;

    inline std::string getFolderId() const
    {
        int p = resource.find("/");
        return resource.substr(0, p);
    }

    inline std::string getFilename() const
    {
        int p = resource.find("/");
        return resource.substr(p + 1);
    }

    inline std::optional<std::string> getFilepath() const { return absolutePath; }

    // Returns the absolute path of the file. If projectOpt is not set the returned path might be relative to the project root
    inline Result<std::string> getFilepath(
        const FolderObject& folder,
        const std::optional<ProjectObject>& projectOpt = std::nullopt) const
    {
        if (absolutePath) return *absolutePath;

        auto folderPath = folder.getFolderpath(projectOpt) err_return;
        if (folderPath == ".") return getFilename();
        std::error_code error;
        auto filename = getFilename();
        if (ghc::filesystem::path(filename).is_absolute()) { return filename; }

        if (StringUtils::endsWith(folderPath, "/")) { return folderPath + filename; }
        return folderPath + "/" + filename;
    }

    void setAbsolutePath(const std::string& path) { absolutePath = path; }

private:
    std::optional<std::string> absolutePath;
};
} // namespace apsync

#endif // ANCHORPOINT_FILE_OBJECT_H
