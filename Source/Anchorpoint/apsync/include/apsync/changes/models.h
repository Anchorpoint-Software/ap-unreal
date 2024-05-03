//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes
#include "apsync/models/event/event_model.h"
#include "apsync/internal/models/access_rule.h"


// Dependencies
#include "apsync/context/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class WorkspaceChange;
class WorkspaceMemberChange;
class GroupChange;
class GroupMemberChange;
class AccountInviteChange;
class WorkspaceInviteChange;
class UserProfileChange;
class SubscriptionChange;
class TrialChange;
class SystemMessageChange;
class ObjectChangeItem;
class ObjectChange;
class DocumentChangeItem;
class DocumentChange;
class EventChange;
class ProgressEvent;

// Aliases
using Change = std::variant< WorkspaceChange, WorkspaceMemberChange, GroupChange, GroupMemberChange, AccountInviteChange, WorkspaceInviteChange, UserProfileChange, SubscriptionChange, TrialChange, SystemMessageChange, ObjectChange, DocumentChange, EventChange, ProgressEvent>;
using ChangeCallback = std::function<Result<void>(std::string, std::vector<Change>)>;

// Enums

// WorkspaceChange
class SYNC_EXPORT WorkspaceChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

public:
    std::string workspaceId;
};

// WorkspaceMemberChange
class SYNC_EXPORT WorkspaceMemberChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

    const std::string& getMemberId() const { return memberId; }

    void setMemberId(const std::string& memberId_) { this->memberId = memberId_; }

public:
    std::string workspaceId;
    std::string memberId;
};

// GroupChange
class SYNC_EXPORT GroupChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

    const std::string& getGroupId() const { return groupId; }

    void setGroupId(const std::string& groupId_) { this->groupId = groupId_; }

public:
    std::string workspaceId;
    std::string groupId;
};

// GroupMemberChange
class SYNC_EXPORT GroupMemberChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

    const std::string& getGroupId() const { return groupId; }

    void setGroupId(const std::string& groupId_) { this->groupId = groupId_; }

    const std::string& getMemberId() const { return memberId; }

    void setMemberId(const std::string& memberId_) { this->memberId = memberId_; }

public:
    std::string workspaceId;
    std::string groupId;
    std::string memberId;
};

// AccountInviteChange
class SYNC_EXPORT AccountInviteChange
{
public:
public:
};

// WorkspaceInviteChange
class SYNC_EXPORT WorkspaceInviteChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

public:
    std::string workspaceId;
};

// UserProfileChange
class SYNC_EXPORT UserProfileChange
{
public:
    const std::string& getUserId() const { return userId; }

    void setUserId(const std::string& userId_) { this->userId = userId_; }

public:
    std::string userId;
};

// SubscriptionChange
class SYNC_EXPORT SubscriptionChange
{
public:
    const std::string& getSubId() const { return subId; }

    void setSubId(const std::string& subId_) { this->subId = subId_; }

public:
    std::string subId;
};

// TrialChange
class SYNC_EXPORT TrialChange
{
public:
    const std::string& getTrialId() const { return trialId; }

    void setTrialId(const std::string& trialId_) { this->trialId = trialId_; }

public:
    std::string trialId;
};

// SystemMessageChange
class SYNC_EXPORT SystemMessageChange
{
public:
    const std::string& getMsgId() const { return msgId; }

    void setMsgId(const std::string& msgId_) { this->msgId = msgId_; }

public:
    std::string msgId;
};

// ObjectChangeItem
class SYNC_EXPORT ObjectChangeItem
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    int64 getRevision() const { return revision; }

    void setRevision(int64 revision_) { this->revision = revision_; }

    bool getDeleted() const { return deleted; }

    void setDeleted(bool deleted_) { this->deleted = deleted_; }

    const std::string& getKind() const { return kind; }

    void setKind(const std::string& kind_) { this->kind = kind_; }

    const std::string& getResource() const { return resource; }

    void setResource(const std::string& resource_) { this->resource = resource_; }

    const std::optional<std::string>& getProjectId() const { return projectId; }

    void setProjectId(const std::optional<std::string>& projectId_) { this->projectId = projectId_; }

    const std::string& getPropsData() const { return propsData; }

    void setPropsData(const std::string& propsData_) { this->propsData = propsData_; }

    const std::vector<apsync::AccessRule>& getAccess() const { return access; }

    void setAccess(const std::vector<apsync::AccessRule>& access_) { this->access = access_; }

    const uint64& getCreatedAt() const { return createdAt; }

    void setCreatedAt(const uint64& createdAt_) { this->createdAt = createdAt_; }

public:
    std::string id;
    int64 revision = 0;
    bool deleted = false;
    std::string kind;
    std::string resource;
    std::optional<std::string> projectId;
    std::string propsData;
    std::vector<apsync::AccessRule> access;
    uint64 createdAt;
};

// ObjectChange
class SYNC_EXPORT ObjectChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

    const std::vector<ObjectChangeItem>& getObjects() const { return objects; }

    void setObjects(const std::vector<ObjectChangeItem>& objects_) { this->objects = objects_; }

public:
    std::string workspaceId;
    std::vector<ObjectChangeItem> objects;
};

// DocumentChangeItem
class SYNC_EXPORT DocumentChangeItem
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    int64 getRevision() const { return revision; }

    void setRevision(int64 revision_) { this->revision = revision_; }

    bool getDeleted() const { return deleted; }

    void setDeleted(bool deleted_) { this->deleted = deleted_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

public:
    std::string id;
    int64 revision = 0;
    bool deleted = false;
    std::string objectId;
};

// DocumentChange
class SYNC_EXPORT DocumentChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

    const std::string& getRepoName() const { return repoName; }

    void setRepoName(const std::string& repoName_) { this->repoName = repoName_; }

    const std::vector<DocumentChangeItem>& getDocuments() const { return documents; }

    void setDocuments(const std::vector<DocumentChangeItem>& documents_) { this->documents = documents_; }

public:
    std::string workspaceId;
    std::string repoName;
    std::vector<DocumentChangeItem> documents;
};

// EventChange
class SYNC_EXPORT EventChange
{
public:
    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

    const std::vector<Event>& getEvents() const { return events; }

    void setEvents(const std::vector<Event>& events_) { this->events = events_; }

public:
    std::string workspaceId;
    std::vector<Event> events;
};

// ProgressEvent
class SYNC_EXPORT ProgressEvent
{
public:
    const std::string& getKind() const { return kind; }

    void setKind(const std::string& kind_) { this->kind = kind_; }

    int64 getPercentage() const { return percentage; }

    void setPercentage(int64 percentage_) { this->percentage = percentage_; }

public:
    std::string kind;
    int64 percentage = 0;
};

} //namespace apsync
//$generated