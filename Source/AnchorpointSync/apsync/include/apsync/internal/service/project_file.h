#ifndef INCLUDE_INTERNAL_SERVICE_PROJECT_FILE
#define INCLUDE_INTERNAL_SERVICE_PROJECT_FILE

#include <optional>
#include <string>

#include "apsync/sync_global.h"

namespace apsync {

struct ProjectFile {
    static constexpr const char* projectFileVersion10 = "1.0";
    static constexpr const char* projectFileVersion11 = "1.1";
    static constexpr const char* projectFileVersion12 = "1.2";
    static constexpr const char* currentProjectFileVersion = projectFileVersion12;
    static constexpr const char* projectShortcutFileExt = ".approj";
    static constexpr const char* templateShortcutFileExt = ".aptemplate";
    std::string workspaceId;
    std::string projectId;
    std::optional<std::string> groupId;
    std::string api;
    std::string version = currentProjectFileVersion;
};

namespace projectfile {
std::optional<ProjectFile> retrieve(const std::string& path, std::string* root = nullptr);
std::optional<ProjectFile> retrieveSingle(const std::string& folderPath, bool legacy = false);
bool create(const std::string& folderPath, const ProjectFile& projectFile,
            const std::optional<std::string>& projectname, bool isTemplate = false,
            bool forceOverwrite = false);
bool migrateShortCut(const std::string& folderPath, const ProjectFile& projectFile,
                     const std::string& projectname, bool isTemplate = false);
std::optional<bool> updateShortCut(const std::string& projectId, const std::string& localPath,
                                   const std::string& newName);

bool remove(const std::string& folderPath);
} // namespace projectfile
} // namespace apsync

#endif /* INCLUDE_INTERNAL_SERVICE_PROJECT_FILE */
