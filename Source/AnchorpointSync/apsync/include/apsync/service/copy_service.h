#ifndef COPY_SERVICE_H
#define COPY_SERVICE_H

#include <functional>
#include <unordered_map>

#include "apsync/models/attributes/database_type.h"
#include "apsync/models/file_object.h"
#include "apsync/models/folder_object.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class ObjectApi;
class DatabaseApi;
class RemoteAttachmentApi;
class ProjectsApi;
class TaskApi;
class ThumbnailService;

enum class VariableIdentifier { SQUARE_BRACKETS = 0, CURLY_BRACKETS };

class CopyServiceImpl;
class SYNC_EXPORT CopyService
{
public:
    CopyService(std::shared_ptr<Api> api);

    Result<void> copyMetadataBatched(
        std::string sourcePath, std::string targetPath,
        const std::unordered_map<std::string, std::string>* variables = nullptr,
        VariableIdentifier identifier = VariableIdentifier::SQUARE_BRACKETS, bool isFile = false,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;

    std::string resolveVariables(
        const std::string& value, const std::unordered_map<std::string, std::string>& variables,
        VariableIdentifier identifier = VariableIdentifier::SQUARE_BRACKETS) const;

    Result<void> copyFolder(
        const std::string& sourcePath, const std::string& targetPath, bool forceOverwrite = false,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;

    Result<void> copyFile(
        const std::string& sourceFilePath, const std::string& targetFilePath,
        bool forceOverwrite = false,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;

    Result<void> copyFolderMetadata(
        const std::string& sourceFolderPath, const std::string& targetFolderPath,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;

    Result<void> copyFileMetadata(
        const std::string& sourceFilePath, const std::string& targetFilePath,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;

    Result<std::string> copyFromTemplate(
        std::string templatePath, std::string targetPath,
        const std::unordered_map<std::string, std::string>& variables, bool skipRoot = true,
        VariableIdentifier identifier = VariableIdentifier::SQUARE_BRACKETS, bool fileOnly = false,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;

    Result<void> renameFile(const std::string& sourcePath, const std::string& targetPath) const;
    Result<void> renameFolder(const std::string& sourcePath, const std::string& targetPath) const;

    Result<void> fileMoved(const FileObject& sourceFile, const std::string& targetFolderPath) const;

    Result<void> clearMetadataForFolder(
        std::string path,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;
    Result<void> clearMetadataForFile(
        std::string path,
        const std::optional<std::string>& activeWorkspaceIdOpt = std::nullopt) const;

private:
    std::shared_ptr<CopyServiceImpl> _impl;

    std::shared_ptr<Api> _api;
    std::shared_ptr<ObjectApi> _objectApi;
    std::shared_ptr<DatabaseApi> _databaseApi;
    std::shared_ptr<RemoteAttachmentApi> _attachmentApi;
    std::shared_ptr<ProjectsApi> _projectsApi;
    std::shared_ptr<TaskApi> _taskApi;
    std::shared_ptr<ThumbnailService> _thumbnailService;
};
} // namespace apsync

#endif // COPY_SERVICE_H
