//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies
#include "apsync/projects/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class ColorConfig;

// Aliases

// Enums

// ColorConfig
class SYNC_EXPORT ColorConfig
{
public:
    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getInProfile() const { return inProfile; }

    void setInProfile(const std::string& inProfile_) { this->inProfile = inProfile_; }

    const std::string& getOutProfile() const { return outProfile; }

    void setOutProfile(const std::string& outProfile_) { this->outProfile = outProfile_; }

    const std::string& getConfigType() const { return configType; }

    void setConfigType(const std::string& configType_) { this->configType = configType_; }

    const std::optional<std::string>& getConfig() const { return config; }

    void setConfig(const std::optional<std::string>& config_) { this->config = config_; }

public:
    std::string objectId;
    std::string inProfile;
    std::string outProfile;
    std::string configType;
    std::optional<std::string> config;
};

} //namespace apsync
//$generated