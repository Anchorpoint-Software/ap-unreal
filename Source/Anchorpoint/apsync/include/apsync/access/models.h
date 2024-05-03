//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class AccessRuleNew;

// Aliases

// Enums

// AccessRuleNew
class SYNC_EXPORT AccessRuleNew
{
public:
    const std::string& getPrincipal() const { return principal; }

    void setPrincipal(const std::string& principal_) { this->principal = principal_; }

    int64 getKlass() const { return klass; }

    void setKlass(int64 klass_) { this->klass = klass_; }

    int64 getLevel() const { return level; }

    void setLevel(int64 level_) { this->level = level_; }

    bool getNegate() const { return negate; }

    void setNegate(bool negate_) { this->negate = negate_; }

    int64 getPermissions() const { return permissions; }

    void setPermissions(int64 permissions_) { this->permissions = permissions_; }

public:
    std::string principal;
    int64 klass = 0;
    int64 level = 0;
    bool negate = false;
    int64 permissions = 0;
};

} //namespace apsync
//$generated