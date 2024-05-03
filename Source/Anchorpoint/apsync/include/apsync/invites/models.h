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
class Invite;

// Aliases

// Enums

// Invite
class SYNC_EXPORT Invite
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getInviterId() const { return inviterId; }

    void setInviterId(const std::string& inviterId_) { this->inviterId = inviterId_; }

    const std::string& getWorkspaceId() const { return workspaceId; }

    void setWorkspaceId(const std::string& workspaceId_) { this->workspaceId = workspaceId_; }

    const std::string& getUserId() const { return userId; }

    void setUserId(const std::string& userId_) { this->userId = userId_; }

    const std::string& getMessage() const { return message; }

    void setMessage(const std::string& message_) { this->message = message_; }

public:
    std::string id;
    std::string inviterId;
    std::string workspaceId;
    std::string userId;
    std::string message;
};

} //namespace apsync
//$generated