#ifndef INCLUDE_MODELS_ACCESS_RULE
#define INCLUDE_MODELS_ACCESS_RULE

#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {
struct AccessRule {
    std::string principal;
    unsigned int class_;
    unsigned int level;
    bool negate;
    unsigned int permissions;

    static AccessRule createWithGroup(const std::string& groupId)
    {
        AccessRule rule;
        rule.principal = groupId;
        rule.class_ = 2; // group
        rule.level = 64; // Writer
        rule.negate = false;
        rule.permissions = 23; // Create, Read, Update, Delete. NO Grant

        return rule;
    }

    static AccessRule createWithUser(const std::string& userId)
    {
        AccessRule rule;
        rule.principal = userId;
        rule.class_ = 0; // user
        rule.level = 64; // Writer
        rule.negate = false;
        rule.permissions = 23; // Create, Read, Update, Delete. NO Grant

        return rule;
    }

    static AccessRule createWithGroupReadOnly(const std::string& groupId)
    {
        AccessRule rule;
        rule.principal = groupId;
        rule.class_ = 2; // group
        rule.level = 64; // Writer
        rule.negate = false;
        rule.permissions = 2; // Read
        return rule;
    }

    static AccessRule createWithGroupAdminReadWrite(const std::string& groupId)
    {
        AccessRule rule;
        rule.principal = groupId;
        rule.class_ = 2; // group
        rule.level = 32; // Admin
        rule.negate = false;
        rule.permissions = 23; // Create, Read, Update, Delete. NO Grant
        return rule;
    }

    static AccessRule createWithUserReadOnly(const std::string& userId)
    {
        AccessRule rule;
        rule.principal = userId;
        rule.class_ = 0; // user
        rule.level = 64; // Writer
        rule.negate = false;
        rule.permissions = 2; // Read
        return rule;
    }

    static AccessRule createWithWorkspaceReadOnly(const std::string& workspaceId)
    {
        AccessRule rule;
        rule.principal = workspaceId;
        rule.class_ = 1; // workspace
        rule.level = 64; // Writer
        rule.negate = false;
        rule.permissions = 2; // Read
        return rule;
    }

    static AccessRule createWithWorkspace(const std::string& workspaceId)
    {
        AccessRule rule;
        rule.principal = workspaceId;
        rule.class_ = 1; // workspace
        rule.level = 64; // Writer
        rule.negate = false;
        rule.permissions = 23; // Create, Read, Update, Delete. NO Grant
        return rule;
    }

    static AccessRule createWithProject(const ProjectObject& project)
    {
        if (project.properties.templateInfo) { return createWithWorkspace(project.workspaceId); }

        return createWithGroup(project.properties.groupId);
    }

    static AccessRule createWithProjectReadOnly(const ProjectObject& project)
    {
        if (project.properties.templateInfo) {
            return createWithWorkspaceReadOnly(project.workspaceId);
        }

        return createWithGroupReadOnly(project.properties.groupId);
    }

    static bool userIsOwner(const std::string& userId,
                            const std::optional<std::vector<AccessRule>>& rules)
    {
        if (!rules) { return false; }

        for (const auto& rule : *rules) {
            if (rule.level == static_cast<unsigned int>(Level::LevelOwner)
                && rule.class_ == 0 /*user*/ && rule.negate == false && userId == rule.principal) {
                return true;
            }
        }

        return false;
    }

    static bool isClassUser(const AccessRule& rule) { return rule.class_ == 0; }

    static bool isLevelOwner(const AccessRule& rule)
    {
        return rule.level == static_cast<unsigned int>(Level::LevelOwner);
    }

    enum class Level {
        LevelOwner = 0,
        LevelAdmin = 32,
        LevelWriter = 64,
        LevelReader = 96,
        LevelNoAccess = 65535
    };

    /*
     if project:
         level 64 (writer)
         permissions 23
         prinicpal: userid
         class: 0
    
     else
         level 64 (writer)
         permissions 23
         prinicpal: groupid
         class: 2

    @see ap-srvc-core/pkg/acs/access.go
    type Class uint16
    const (
        ClassUser Class = iota
        ClassWorkspace
        ClassGroup
        )
        
    type Level uint16
    const (
        LevelOwner Level = 32 * iota
        LevelAdmin
        LevelWriter
        LevelReader
        LevelNoAccess Level = math.MaxUint16
        )
        
    type PermissionMask byte
    const (
        CanCreate PermissionMask = 1 << iota // 1
        CanRead                              // 2
        CanUpdate                            // 4
        CanGrant                             // 8
        CanDelete                            // 16
        )
*/
};
} // namespace apsync

#endif /* INCLUDE_MODELS_ACCESS_RULE */
