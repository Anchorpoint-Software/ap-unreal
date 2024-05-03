//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT ProjectsApi
{
public:
    explicit ProjectsApi(std::shared_ptr<Api> api);

    Result<ProjectObject> createProject(
        const std::optional<apsync::Url>& localPath,
        const std::string& name,
        const std::optional<std::string>& workspaceId,
        const std::optional<apsync::Url>& iconPath,
        const std::optional<TemplateInfo>& templateInfo,
        bool forceOverwrite
    );

    Result<void> createProjectFile(
        const std::string& projectId,
        const std::string& workspaceId,
        const apsync::Url& localPath,
        const std::optional<std::string>& name,
        bool forceOverwrite
    );

    Result<void> migrateProjectShortcutFile(
        const std::string& projectId,
        const std::string& workspaceId,
        const apsync::Url& localPath,
        const std::string& name
    );

    Result<void> deleteProject(
        const apsync::Url& localPath
    );

    Result<void> deleteProject(
        const std::string& projectId,
        const std::optional<apsync::Url>& localPath,
        const std::string& workspaceId
    );

    Result<std::vector<ProjectObject>> listProjects(
        const std::string& workspaceId,
        bool sync
    );

    Result<void> setProjectName(
        const std::string& projectId,
        const std::string& name,
        const std::string& workspaceId,
        const std::optional<apsync::Url>& localPath
    );

    Result<apsync::Url> setProjectIcon(
        const std::string& projectId,
        const apsync::Url& iconPath,
        const std::string& workspaceId
    );

    Result<void> clearProjectIcon(
        const std::string& projectId,
        const std::string& workspaceId
    );

    Result<void> setProjectDescription(
        const std::string& projectId,
        const std::optional<std::string>& description,
        const std::string& workspaceId
    );

    Result<ProjectObject> setProjectMetadata(
        const std::string& projectId,
        const std::unordered_map<std::string,std::string>& metadata,
        const std::string& workspaceId
    );

    Result<void> attachPathToProject(
        const std::string& projectId,
        const std::string& workspaceId,
        const apsync::Url& localPath,
        bool forceOverwrite
    );

    Result<ProjectObject> getProject(
        const std::string& projectId,
        const std::string& workspaceId
    );

    Result<ProjectObject> getProjectForPath(
        const apsync::Url& path
    );

    Result<std::optional<ProjectObject>> getProjectByPath(
        const std::string& path
    );

    Result<bool> isProject(
        const apsync::Url& path,
        bool recursive
    );

    Result<std::string> getProjectGroup(
        const std::string& projectId,
        const std::string& workspaceId
    );

    Result<void> addMemberWithNoAccess(
        const std::string& workspaceId,
        const std::string& groupId,
        const std::string& userId
    );

    Result<std::string> exportProject(
        const std::string& projectId,
        const std::string& workspaceId,
        const apsync::Url& targetFolderPath
    );

    Result<ProjectObject> importProject(
        const apsync::Url& exchangeFilePath,
        const std::string& targetWorkspaceId,
        const apsync::Url& projectRootPath
    );

    Result<ProjectObject> archiveProject(
        const std::string& projectId,
        const std::string& workspaceId,
        const ProjectArchiveInfo& archiveInfo
    );

    Result<ProjectObject> restoreProject(
        const std::string& projectId,
        const std::string& workspaceId
    );

private:
    std::shared_ptr<class ProjectsApiImpl> _impl;
};

} // namespace apsync
//$generated