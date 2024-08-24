//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies
#include "apsync/users/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class Group;
class UserGroup;
class GroupMember;

// Aliases

// Enums

// Group
class SYNC_EXPORT Group
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

    const std::string& getKind() const { return kind; }

    void setKind(const std::string& kind_) { this->kind = kind_; }

public:
    std::string id;
    std::string name;
    std::string kind;
};

// UserGroup
class SYNC_EXPORT UserGroup
{
public:
    const Group& getGroup() const { return group; }

    void setGroup(const Group& group_) { this->group = group_; }

    int64 getLevel() const { return level; }

    void setLevel(int64 level_) { this->level = level_; }

public:
    Group group;
    int64 level = 0;
};

// GroupMember
class SYNC_EXPORT GroupMember
{
public:
    const std::string& getUserId() const { return userId; }

    void setUserId(const std::string& userId_) { this->userId = userId_; }

    int64 getLevel() const { return level; }

    void setLevel(int64 level_) { this->level = level_; }

    const std::string& getEmail() const { return email; }

    void setEmail(const std::string& email_) { this->email = email_; }

    const UserProfileNew& getProfile() const { return profile; }

    void setProfile(const UserProfileNew& profile_) { this->profile = profile_; }

public:
    std::string userId;
    int64 level = 0;
    std::string email;
    UserProfileNew profile;
};

} //namespace apsync
//$generated