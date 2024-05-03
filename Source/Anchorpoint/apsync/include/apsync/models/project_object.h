#ifndef INCLUDE_MODELS_PROJECT
#define INCLUDE_MODELS_PROJECT

#include <map>
#include <vector>

#include "base_object.h"
#include "template/template.h"

namespace apsync {
using ProjectMetadata = std::unordered_map<std::string, std::string>;

struct SYNC_EXPORT ProjectArchiveInfo {

    std::string userId;
    uint64 date;
};

struct SYNC_EXPORT ProjectObjectProps {
    ProjectObjectProps() = default;
    ProjectObjectProps(const std::string& name, const std::string& groupId,
                       const std::optional<std::string>& desc = std::nullopt)
        : name(name),
          groupId(groupId),
          desc(desc)
    {}
    std::string name;
    std::string groupId;
    std::optional<std::string> desc;
    ProjectMetadata metadata;
    std::vector<std::string> possiblePaths;
    std::optional<Url> iconPath;
    std::optional<ProjectArchiveInfo> archiveInfo;
    std::optional<TemplateInfo> templateInfo;

    //always true for version 0
    bool hasAttachedPath;

    static constexpr const int currentVersion = 1;
    int version = 0;
};

struct SYNC_EXPORT ProjectObject: public BaseObject {
    ProjectObject() : BaseObject(ObjectType::PROJECT){};
    ProjectObject(const std::string& id, const std::string& workspaceId, uint64 createdAt,
                  const ProjectObjectProps& properties)
        : BaseObject(id, workspaceId, createdAt, ObjectType::PROJECT),
          properties(properties)
    {}

    std::optional<std::string> rootPath;
    ProjectObjectProps properties;
};
} // namespace apsync

#endif /* INCLUDE_MODELS_PROJECT */
