//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes
#include "apsync/models/icon/icon_model.h"
#include "apsync/models/project_object.h"


// Dependencies
#include "apsync/projects/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class TimelineChannel;

// Aliases

// Enums

// TimelineChannel
class SYNC_EXPORT TimelineChannel
{
public:
    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getIdentifier() const { return identifier; }

    void setIdentifier(const std::string& identifier_) { this->identifier = identifier_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

    const std::optional<Icon>& getIcon() const { return icon; }

    void setIcon(const std::optional<Icon>& icon_) { this->icon = icon_; }

    const std::unordered_map<std::string, std::string>& getMetadata() const { return metadata; }

    void setMetadata(const std::unordered_map<std::string, std::string>& metadata_) { this->metadata = metadata_; }

public:
    std::string objectId;
    std::string identifier;
    std::string name;
    std::optional<Icon> icon;
    std::unordered_map<std::string, std::string> metadata;
};

} //namespace apsync
//$generated