#ifndef INC_VERSION_SERVICE_H
#define INC_VERSION_SERVICE_H

#include <unordered_map>
#include <vector>

#include "apsync/models/file_object.h"
#include "apsync/models/folder_object.h"
#include "apsync/models/project_object.h"
#include "apsync/models/versioning/version_entry.h"
#include "apsync/models/versioning/versioned_object.h"
#include "apsync/sync_global.h"

namespace apsync {

class Api;
class IncVersionServiceImpl;
class SYNC_EXPORT IncVersionService
{
public:
    explicit IncVersionService(std::shared_ptr<Api> api);

    Result<void> toggleVersionControl(FolderObject& folderObject, bool enable);

    Result<std::vector<VersionedObject>> getVersionedObjects(
        const std::string& folderPath, const std::optional<std::string>& workspaceId,
        const std::optional<ProjectObject>& project);
    Result<std::vector<VersionedObject>> identifyVersionedObjects(
        const std::string& folderPath, const std::optional<std::string>& workspaceId,
        const std::optional<ProjectObject>& project);

    Result<std::optional<VersionedObject>> getVersionedObject(
        const std::string& filePath, const std::optional<std::string>& workspaceId,
        const std::optional<ProjectObject>& project);
    Result<std::optional<VersionedObject>> identifyVersionedObject(
        const std::string& filePath, const std::optional<std::string>& workspaceId,
        const std::optional<ProjectObject>& project);

    Result<std::string> getNextVersionName(const VersionedObject& object) const;
    Result<std::string> createNextVersion(VersionedObject& object,
                                          const std::optional<ProjectObject>& projectOpt,
                                          const std::optional<VersionEntry>& parent = std::nullopt,
                                          const std::optional<std::string>& userId = std::nullopt,
                                          std::optional<uint64> date = std::nullopt);
    Result<void> addVersions(VersionedObject& object, const std::vector<FileObject> fileObjects,
                             const std::optional<std::string>& userId = std::nullopt);
    Result<void> removeVersion(VersionedObject& object, VersionEntry& entry,
                               bool permanent = false);

    std::unordered_map<std::string, std::string> previewRenameVersions(
        const VersionedObject& object);
    Result<void> renameVersions(VersionedObject& object);
    Result<void> renameVersionBase(VersionedObject& object, const std::string& targetBaseName,
                                   bool applyRename = true);

    Result<void> moveVersionEntry(VersionedObject& object, int currentVersionNumber,
                                  int targetVersionNumber);

    Result<void> setParent(VersionedObject& object, VersionEntry& entry,
                           const std::optional<VersionEntry>& parent);

    Result<void> publish(VersionedObject& object, VersionEntry& entry,
                         const std::optional<std::string>& userId = std::nullopt,
                         const std::string& extension = "published");

    Result<void> commentVersion(VersionedObject& object, VersionEntry& entry,
                                const std::optional<std::string>& comment,
                                const std::optional<std::string>& userId = std::nullopt,
                                std::optional<uint64> date = std::nullopt);

    Result<std::string> resolveVersionFilestackBasefile(const FileObject& fileObject,
                                                        bool& isFilestack) const;
    Result<std::string> resolveImageFilestackBasefile(const std::string& filePath,
                                                      bool& isFilestack,
                                                      IncrementStyle& style) const;

    std::string getFormatExtension(IncrementStyle style, bool withFillChars = false) const;

private:
    std::shared_ptr<Api> _api;
    std::shared_ptr<IncVersionServiceImpl> _pimpl;
};
} // namespace apsync
#endif // INC_VERSION_SERVICE_H
