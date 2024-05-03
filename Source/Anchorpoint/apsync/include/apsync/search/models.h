//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies
#include "apsync/context/models.h"
#include "apsync/attributes/models.h"
#include "apsync/files/models.h"
#include "apsync/filters/models.h"
#include "apsync/tasks/models.h"
#include "apsync/projects/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class SearchOptions;
class NameMatch;
class PathMatch;
class AttributeMatch;
class FileSearchResult;
class RemoteFileSearchResult;
class FolderSearchResult;
class RemoteFolderSearchResult;
class TaskSearchResult;
class TaskListSearchResult;
class AttributeSearchResult;

// Aliases
using AnySearchResult = std::variant< FileSearchResult, RemoteFileSearchResult, FolderSearchResult, RemoteFolderSearchResult, TaskSearchResult, TaskListSearchResult, AttributeSearchResult>;
using AnyMatch = std::variant<AttributeMatch,NameMatch,PathMatch>;
using SearchDelegate = std::function<Result<void>(std::vector<AnySearchResult>)>;

// Enums

// SearchOptions
class SYNC_EXPORT SearchOptions
{
public:
    bool getWithFiles() const { return withFiles; }

    void setWithFiles(bool withFiles_) { this->withFiles = withFiles_; }

    bool getWithFolders() const { return withFolders; }

    void setWithFolders(bool withFolders_) { this->withFolders = withFolders_; }

    bool getWithTasks() const { return withTasks; }

    void setWithTasks(bool withTasks_) { this->withTasks = withTasks_; }

    bool getWithTaskLists() const { return withTaskLists; }

    void setWithTaskLists(bool withTaskLists_) { this->withTaskLists = withTaskLists_; }

    bool getWithAttributes() const { return withAttributes; }

    void setWithAttributes(bool withAttributes_) { this->withAttributes = withAttributes_; }

    bool getWithHidden() const { return withHidden; }

    void setWithHidden(bool withHidden_) { this->withHidden = withHidden_; }

    const std::optional<PathFilter>& getPathFilter() const { return pathFilter; }

    void setPathFilter(const std::optional<PathFilter>& pathFilter_) { this->pathFilter = pathFilter_; }

    const std::optional<DateFilterNew>& getDateFilter() const { return dateFilter; }

    void setDateFilter(const std::optional<DateFilterNew>& dateFilter_) { this->dateFilter = dateFilter_; }

public:
    bool withFiles = true;
    bool withFolders = true;
    bool withTasks = true;
    bool withTaskLists = true;
    bool withAttributes = true;
    bool withHidden = false;
    std::optional<PathFilter> pathFilter;
    std::optional<DateFilterNew> dateFilter;
};

// NameMatch
class SYNC_EXPORT NameMatch
{
public:
    const std::string& getToken() const { return token; }

    void setToken(const std::string& token_) { this->token = token_; }

    bool getExact() const { return exact; }

    void setExact(bool exact_) { this->exact = exact_; }

public:
    std::string token;
    bool exact = false;
};

// PathMatch
class SYNC_EXPORT PathMatch
{
public:
    const std::string& getToken() const { return token; }

    void setToken(const std::string& token_) { this->token = token_; }

    bool getExact() const { return exact; }

    void setExact(bool exact_) { this->exact = exact_; }

public:
    std::string token;
    bool exact = false;
};

// AttributeMatch
class SYNC_EXPORT AttributeMatch
{
public:
    const std::string& getToken() const { return token; }

    void setToken(const std::string& token_) { this->token = token_; }

    bool getExact() const { return exact; }

    void setExact(bool exact_) { this->exact = exact_; }

public:
    std::string token;
    bool exact = false;
};

// FileSearchResult
class SYNC_EXPORT FileSearchResult
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const File& getFile() const { return file; }

    void setFile(const File& file_) { this->file = file_; }

    const Folder& getParent() const { return parent; }

    void setParent(const Folder& parent_) { this->parent = parent_; }

    const std::vector<AnyMatch>& getMatches() const { return matches; }

    void setMatches(const std::vector<AnyMatch>& matches_) { this->matches = matches_; }

public:
    std::string path;
    File file;
    Folder parent;
    std::vector<AnyMatch> matches;
};

// RemoteFileSearchResult
class SYNC_EXPORT RemoteFileSearchResult
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const Folder& getParent() const { return parent; }

    void setParent(const Folder& parent_) { this->parent = parent_; }

    const std::vector<AnyMatch>& getMatches() const { return matches; }

    void setMatches(const std::vector<AnyMatch>& matches_) { this->matches = matches_; }

public:
    std::string path;
    Folder parent;
    std::vector<AnyMatch> matches;
};

// FolderSearchResult
class SYNC_EXPORT FolderSearchResult
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const Folder& getFolder() const { return folder; }

    void setFolder(const Folder& folder_) { this->folder = folder_; }

    const Folder& getParent() const { return parent; }

    void setParent(const Folder& parent_) { this->parent = parent_; }

    const std::vector<AnyMatch>& getMatches() const { return matches; }

    void setMatches(const std::vector<AnyMatch>& matches_) { this->matches = matches_; }

public:
    std::string path;
    Folder folder;
    Folder parent;
    std::vector<AnyMatch> matches;
};

// RemoteFolderSearchResult
class SYNC_EXPORT RemoteFolderSearchResult
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const Folder& getFolder() const { return folder; }

    void setFolder(const Folder& folder_) { this->folder = folder_; }

    const Folder& getParent() const { return parent; }

    void setParent(const Folder& parent_) { this->parent = parent_; }

    const std::vector<AnyMatch>& getMatches() const { return matches; }

    void setMatches(const std::vector<AnyMatch>& matches_) { this->matches = matches_; }

public:
    std::string path;
    Folder folder;
    Folder parent;
    std::vector<AnyMatch> matches;
};

// TaskSearchResult
class SYNC_EXPORT TaskSearchResult
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const Task& getTask() const { return task; }

    void setTask(const Task& task_) { this->task = task_; }

    const Folder& getParent() const { return parent; }

    void setParent(const Folder& parent_) { this->parent = parent_; }

    const std::vector<AnyMatch>& getMatches() const { return matches; }

    void setMatches(const std::vector<AnyMatch>& matches_) { this->matches = matches_; }

public:
    std::string path;
    Task task;
    Folder parent;
    std::vector<AnyMatch> matches;
};

// TaskListSearchResult
class SYNC_EXPORT TaskListSearchResult
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const Folder& getParent() const { return parent; }

    void setParent(const Folder& parent_) { this->parent = parent_; }

    int64 getChangedAt() const { return changedAt; }

    void setChangedAt(int64 changedAt_) { this->changedAt = changedAt_; }

    const std::vector<AnyMatch>& getMatches() const { return matches; }

    void setMatches(const std::vector<AnyMatch>& matches_) { this->matches = matches_; }

public:
    std::string path;
    std::string id;
    Folder parent;
    int64 changedAt = 0;
    std::vector<AnyMatch> matches;
};

// AttributeSearchResult
class SYNC_EXPORT AttributeSearchResult
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    const std::variant<File, Folder, Task>& getObject() const { return object; }

    void setObject(const std::variant<File, Folder, Task>& object_) { this->object = object_; }

    const Folder& getParent() const { return parent; }

    void setParent(const Folder& parent_) { this->parent = parent_; }

    const std::vector<AttributeWithColumn>& getAttributes() const { return attributes; }

    void setAttributes(const std::vector<AttributeWithColumn>& attributes_) { this->attributes = attributes_; }

    const std::vector<AnyMatch>& getMatches() const { return matches; }

    void setMatches(const std::vector<AnyMatch>& matches_) { this->matches = matches_; }

public:
    std::string path;
    std::variant<File, Folder, Task> object;
    Folder parent;
    std::vector<AttributeWithColumn> attributes;
    std::vector<AnyMatch> matches;
};

} //namespace apsync
//$generated