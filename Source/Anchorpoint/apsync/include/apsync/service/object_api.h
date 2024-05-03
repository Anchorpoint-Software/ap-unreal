#ifndef ANCHORPOINT_OBJECT_API_H
#define ANCHORPOINT_OBJECT_API_H

#include <unordered_map>

#include "apsync/models/file_object.h"
#include "apsync/models/folder_object.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {
struct Object;
class Api;
class ObjectApiImpl;
class SYNC_EXPORT ObjectApi
{
public:
    explicit ObjectApi(std::shared_ptr<Api> api);

    Result<BaseObject> getBaseObject(const std::string& objectId,
                                     const std::string& workspaceId) const;

    Result<FolderObject> getFolderObjectById(const std::string& id,
                                             const std::string& workspaceId) const;

    Result<std::optional<FolderObject>> getFolderObject(
        const std::string& path, const std::optional<std::string>& workspaceId,
        const std::optional<ProjectObject>& project) const;

    Result<std::vector<FileObject>> getFileObjects(const FolderObject& parentFolder,
                                                   const std::vector<std::string>& filepaths) const;
    Result<std::vector<FileObject>> identifyFileObjects(
        const FolderObject& parentFolder, const std::vector<std::string>& filepaths) const;
    Result<std::vector<FolderObject>> identifyFolderObjects(
        const vector<std::string>& folderpaths, const std::string& workspaceId,
        const std::optional<ProjectObject>& projectOpt) const;

    Result<std::vector<FileObject>> getAllFileObjectsFromFolder(
        const FolderObject& parentFolder, const std::optional<ProjectObject>& projectOpt) const;

    Result<std::vector<FolderObject>> getFolderObjects(
        const std::vector<std::string>& folderpaths, const std::string& workspaceId,
        const std::optional<ProjectObject>& projectOpt) const;

    Result<std::vector<FolderObject>> getAllFolderObjectsFromFolder(
        const FolderObject& parentFolder, const std::optional<ProjectObject>& projectOpt,
        bool recursive = false) const;

    Result<void> update(const FileObject& object);
    Result<void> update(const FolderObject& object);
    Result<FileObject> rename(const FileObject& object, const std::string& newName);
    Result<FileObject> move(const FileObject& object, const FolderObject& newFolder);
    Result<FolderObject> rename(const FolderObject& object, const std::string& newName);

    Result<FileObject> copy(const FileObject& object, const std::string& targetPath,
                            const std::string& targetWorkspaceId,
                            const std::optional<ProjectObject>& projectOpt);
    Result<FileObject> copy(const FileObject& object, const std::string& targetPath,
                            const std::string& targetWorkspaceId,
                            const std::optional<std::string>& projectFolder,
                            const std::optional<std::string>& projectId);
    Result<FolderObject> copy(const FolderObject& object, const std::string& targetPath,
                              const std::string& targetWorkspaceId,
                              const std::optional<std::string>& projectFolder,
                              const std::optional<std::string>& projectId);

    Result<void> resync(const std::string& workspaceId);
    Result<void> dropLocal(const std::string& workspaceId);
    Result<void> rescanAccess(const std::string& workspaceId, const std::string& accessId);

    Result<void> syncOffline(const std::string& workspaceId);

    Result<std::string> resolvePath(const std::optional<ProjectObject>& projectObject,
                                    const std::string& objectId, const std::string& workspaceId,
                                    const ObjectType& type,
                                    std::optional<FileObject>* fileObjectOpt = nullptr,
                                    std::optional<FolderObject>* folderObjectOpt = nullptr);

    Result<void> updateFileIcons(const std::vector<std::string>& filePaths,
                                 const std::optional<std::string>& iconPath,
                                 const std::optional<std::string>& iconColor,
                                 const std::string& workspaceId,
                                 const std::optional<ProjectObject>& project);
    Result<void> updateFolderIcons(std::vector<std::string> paths,
                                   const std::optional<std::string>& iconPath,
                                   const std::optional<std::string>& iconColor,
                                   const std::string& workspaceId,
                                   const std::optional<ProjectObject>& project);

    Result<void> setFileIcons(const std::vector<std::string>& filePaths,
                              const std::optional<Icon>& icon, const std::string& workspaceId,
                              const std::optional<ProjectObject>& project);
    Result<void> setFolderIcons(std::vector<std::string> paths, const std::optional<Icon>& icon,
                                const std::string& workspaceId,
                                const std::optional<ProjectObject>& project);
    Result<void> setFolderIcon(FolderObject& object, const std::optional<Icon>& icon);


    // Deprecated
    Result<std::optional<FileObject>> getFileObject(
        const std::string& path, const std::optional<std::string>& workspaceId,
        const std::optional<ProjectObject>& project) const;
    Result<std::optional<FolderObject>> getFolderObject(const std::string& absolutePath) const;
    Result<std::optional<FolderObject>> getFolderObject(const std::string& relativePath,
                                                        const std::string& projectFolder,
                                                        const std::string& workspaceId,
                                                        const std::string& projectId) const;


    Result<FolderObject> identifyFolder(const std::string& path,
                                        const std::optional<std::string>& workspaceId,
                                        const std::optional<ProjectObject>& project);
    Result<FolderObject> identifyFolder(const std::string& absolutePath);
    Result<FolderObject> identifyFolder(const std::string& relativePath,
                                        const std::string& projectFolder,
                                        const std::string& workspaceId,
                                        const std::string& projectId);

    Result<FileObject> getFileObjectById(const std::string& id,
                                         const std::string& workspaceId) const;
    Result<std::optional<FileObject>> getFileObject(const std::string& absolutePath) const;
    Result<std::optional<FileObject>> getFileObject(const std::string& relativePath,
                                                    const std::string& projectFolder,
                                                    const std::string& workspaceId,
                                                    const std::string& projectId) const;

    Result<FileObject> identifyFile(const std::string& absolutePath);
    Result<FileObject> identifyFile(const std::string& relativePath,
                                    const std::string& projectFolder,
                                    const std::string& workspaceId, const std::string& projectId);
    Result<FileObject> identifyFile(const std::string& path,
                                    const std::optional<std::string>& workspaceId,
                                    const std::optional<ProjectObject>& project);

private:
    Result<std::unordered_map<std::string, FileObject>> getFileObjects(
        const std::string& commonFolder, const std::vector<std::string>& filenames,
        const std::string& workspaceId) const;
    Result<std::unordered_map<std::string, FileObject>> getFileObjects(
        const std::string& commonRelativeFolder, const std::vector<std::string>& filenames,
        const std::string& projectFolder, const std::string& workspaceId,
        const std::string& projectId) const;
    Result<std::unordered_map<std::string, FileObject>> getFileObjects(
        const std::string& commonFolder, const std::vector<std::string>& filenames,
        const std::string& workspaceId, const std::optional<ProjectObject>& project) const;

    Result<std::unordered_map<std::string, FolderObject>> getFolderObjects(
        const std::vector<std::string>& absolutePaths, const std::string& workspaceId) const;
    Result<std::unordered_map<std::string, FolderObject>> getFolderObjects(
        const std::vector<std::string>& relativePaths, const std::string& workspaceId,
        const std::string& projectId) const;


    Result<std::optional<FolderObject>> getFolderObjectNoProject(
        const std::string& absolutePath, const std::string& workspaceId) const;
    Result<FolderObject> identifyFolderNoProject(const std::string& absolutePath,
                                                 const std::string& workspaceId);
    Result<std::optional<FileObject>> getFileObjectNoProject(const std::string& absolutePath,
                                                             const std::string& workspaceId) const;
    Result<FileObject> identifyFileNoProject(const std::string& absolutePath,
                                             const std::string& workspaceId);

    std::shared_ptr<ObjectApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
};

extern Result<Object> getObjectFromList(const std::vector<Object>& objects);
extern void to_json(nlohmann::json& nlohmann_json_j, const FolderObjectProps& nlohmann_json_t);
void from_json(const nlohmann::json& nlohmann_json_j, FolderObjectProps& nlohmann_json_t);

} // namespace apsync

#endif // ANCHORPOINT_OBJECT_API_H
