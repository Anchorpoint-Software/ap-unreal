//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies
#include "apsync/context/models.h"
#include "apsync/attributes/models.h"
#include "apsync/filters/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class File;
class Folder;
class FileEntryWithAttributes;
class ListFilesOptions;

// Aliases
using AnyListFilesFilter = std::variant<NameFilter, PathFilter, AnyAttributeFilter>;
using ListFilesDelegate = std::function<Result<void>(std::vector<FileEntryWithAttributes>)>;

// Enums

// File
class SYNC_EXPORT File
{
public:
    bool getIsVirtual() const { return isVirtual; }

    void setIsVirtual(bool isVirtual_) { this->isVirtual = isVirtual_; }

    bool getIsHidden() const { return isHidden; }

    void setIsHidden(bool isHidden_) { this->isHidden = isHidden_; }

    int64 getChangedAt() const { return changedAt; }

    void setChangedAt(int64 changedAt_) { this->changedAt = changedAt_; }

public:
    bool isVirtual = false;
    bool isHidden = false;
    int64 changedAt = 0;
};

// Folder
class SYNC_EXPORT Folder
{
public:
    const std::optional<Icon2>& getIcon() const { return icon; }

    void setIcon(const std::optional<Icon2>& icon_) { this->icon = icon_; }

    bool getIsHidden() const { return isHidden; }

    void setIsHidden(bool isHidden_) { this->isHidden = isHidden_; }

    int64 getChangedAt() const { return changedAt; }

    void setChangedAt(int64 changedAt_) { this->changedAt = changedAt_; }

public:
    std::optional<Icon2> icon;
    bool isHidden = false;
    int64 changedAt = 0;
};

// FileEntryWithAttributes
class SYNC_EXPORT FileEntryWithAttributes
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const std::variant<File, Folder>& getEntry() const { return entry; }

    void setEntry(const std::variant<File, Folder>& entry_) { this->entry = entry_; }

    const std::vector<AttributeWithColumn>& getAttributes() const { return attributes; }

    void setAttributes(const std::vector<AttributeWithColumn>& attributes_) { this->attributes = attributes_; }

public:
    std::string path;
    std::variant<File, Folder> entry;
    std::vector<AttributeWithColumn> attributes;
};

// ListFilesOptions
class SYNC_EXPORT ListFilesOptions
{
public:
    bool getWithFiles() const { return withFiles; }

    void setWithFiles(bool withFiles_) { this->withFiles = withFiles_; }

    bool getWithFolders() const { return withFolders; }

    void setWithFolders(bool withFolders_) { this->withFolders = withFolders_; }

    bool getWithHidden() const { return withHidden; }

    void setWithHidden(bool withHidden_) { this->withHidden = withHidden_; }

    const std::vector<AnyListFilesFilter>& getFilters() const { return filters; }

    void setFilters(const std::vector<AnyListFilesFilter>& filters_) { this->filters = filters_; }

public:
    bool withFiles = true;
    bool withFolders = true;
    bool withHidden = false;
    std::vector<AnyListFilesFilter> filters;
};

} //namespace apsync
//$generated