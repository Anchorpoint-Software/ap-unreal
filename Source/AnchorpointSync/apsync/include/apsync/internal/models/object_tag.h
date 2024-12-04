#ifndef OBJECT_TAG_H
#define OBJECT_TAG_H

#include <unordered_map>

#include "apsync/models/project_object.h"

namespace apsync {
namespace tag {
using Key = std::string;
using Tag = std::string;
using Tags = std::unordered_map<Key, Tag>;

static void addProject(Tags& tags, const std::string& projectId) { tags["pid"] = projectId; }


static void addProject(Tags& tags, const std::optional<apsync::ProjectObject>& projectOpt)
{
    if (projectOpt) addProject(tags, projectOpt->id);
}


static std::optional<std::string> getProjectId(const std::optional<Tags>& tags)
{
    if (!tags) return std::nullopt;
    auto it = tags->find("pid");
    if (it != tags->end()) return it->second;
    return std::nullopt;
}

} // namespace tag
} // namespace apsync

#endif // OBJECT_TAG_H
