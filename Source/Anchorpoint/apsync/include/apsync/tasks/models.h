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
class Task;
class TaskEntryWithAttributes;
class ListTasksOptions;

// Aliases
using AnyListTasksFilter = std::variant<NameFilter, PathFilter, AnyAttributeFilter>;
using ListTasksDelegate = std::function<Result<void>(std::vector<TaskEntryWithAttributes>)>;

// Enums

// Task
class SYNC_EXPORT Task
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

    const std::string& getTaskListID() const { return taskListID; }

    void setTaskListID(const std::string& taskListID_) { this->taskListID = taskListID_; }

    const std::optional<Icon2>& getIcon() const { return icon; }

    void setIcon(const std::optional<Icon2>& icon_) { this->icon = icon_; }

    int64 getAttachmentCount() const { return attachmentCount; }

    void setAttachmentCount(int64 attachmentCount_) { this->attachmentCount = attachmentCount_; }

    int64 getCreatedAt() const { return createdAt; }

    void setCreatedAt(int64 createdAt_) { this->createdAt = createdAt_; }

public:
    std::string id;
    std::string name;
    std::string taskListID;
    std::optional<Icon2> icon;
    int64 attachmentCount = 0;
    int64 createdAt = 0;
};

// TaskEntryWithAttributes
class SYNC_EXPORT TaskEntryWithAttributes
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const Task& getEntry() const { return entry; }

    void setEntry(const Task& entry_) { this->entry = entry_; }

    const std::vector<AttributeWithColumn>& getAttributes() const { return attributes; }

    void setAttributes(const std::vector<AttributeWithColumn>& attributes_) { this->attributes = attributes_; }

public:
    std::string path;
    Task entry;
    std::vector<AttributeWithColumn> attributes;
};

// ListTasksOptions
class SYNC_EXPORT ListTasksOptions
{
public:
    const std::vector<AnyListTasksFilter>& getFilters() const { return filters; }

    void setFilters(const std::vector<AnyListTasksFilter>& filters_) { this->filters = filters_; }

public:
    std::vector<AnyListTasksFilter> filters;
};

} //namespace apsync
//$generated