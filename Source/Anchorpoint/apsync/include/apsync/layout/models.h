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
class LayoutEntry;
class Layout;

// Aliases

// Enums

// LayoutEntry
class SYNC_EXPORT LayoutEntry
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getKind() const { return kind; }

    void setKind(const std::string& kind_) { this->kind = kind_; }

    const std::optional<std::string>& getLinkedId() const { return linkedId; }

    void setLinkedId(const std::optional<std::string>& linkedId_) { this->linkedId = linkedId_; }

    bool getFolded() const { return folded; }

    void setFolded(bool folded_) { this->folded = folded_; }

public:
    std::string id;
    std::string kind;
    std::optional<std::string> linkedId;
    bool folded = false;
};

// Layout
class SYNC_EXPORT Layout
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::vector<LayoutEntry>& getEntries() const { return entries; }

    void setEntries(const std::vector<LayoutEntry>& entries_) { this->entries = entries_; }

    const std::optional<std::string>& getLinkedId() const { return linkedId; }

    void setLinkedId(const std::optional<std::string>& linkedId_) { this->linkedId = linkedId_; }

public:
    std::string id;
    std::vector<LayoutEntry> entries;
    std::optional<std::string> linkedId;
};

} //namespace apsync
//$generated