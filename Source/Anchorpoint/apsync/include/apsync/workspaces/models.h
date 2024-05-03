//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies
#include "apsync/access/models.h"
#include "apsync/quotas/models.h"
#include "apsync/invites/models.h"
#include "apsync/users/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class UserWorkspace;
class WorkspaceMember;

// Aliases

// Enums
enum class WorkspaceTier : uint64
{
    FREE = 12529378285943743416ULL,
    PRO = 11599087218745796166ULL,
    PRO2024 = 8973214501205454027ULL,
    TEAM = 5452169976120677090ULL,
    ORG = 3319632522302897076ULL
};


// UserWorkspace
class SYNC_EXPORT UserWorkspace
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

    const std::string& getKind() const { return kind; }

    void setKind(const std::string& kind_) { this->kind = kind_; }

    int64 getLevel() const { return level; }

    void setLevel(int64 level_) { this->level = level_; }

    const std::unordered_map<std::string,QuotaValue>& getQuotas() const { return quotas; }

    void setQuotas(const std::unordered_map<std::string,QuotaValue>& quotas_) { this->quotas = quotas_; }

    const WorkspaceTier& getTier() const { return tier; }

    void setTier(const WorkspaceTier& tier_) { this->tier = tier_; }

    int64 getTrialExpiresAt() const { return trialExpiresAt; }

    void setTrialExpiresAt(int64 trialExpiresAt_) { this->trialExpiresAt = trialExpiresAt_; }

public:
    std::string id;
    std::string name;
    std::string kind;
    int64 level = 0;
    std::unordered_map<std::string,QuotaValue> quotas;
    WorkspaceTier tier;
    int64 trialExpiresAt = 0;
};

// WorkspaceMember
class SYNC_EXPORT WorkspaceMember
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

    bool getPending() const { return pending; }

    void setPending(bool pending_) { this->pending = pending_; }

public:
    std::string userId;
    int64 level = 0;
    std::string email;
    UserProfileNew profile;
    bool pending = false;
};

} //namespace apsync
//$generated