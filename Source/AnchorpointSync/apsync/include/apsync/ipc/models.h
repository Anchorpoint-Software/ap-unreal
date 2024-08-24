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
class IpcMessage;
class IpcSubscription;

// Aliases

// Enums

// IpcMessage
class SYNC_EXPORT IpcMessage
{
public:
    const std::string& getSenderId() const { return senderId; }

    void setSenderId(const std::string& senderId_) { this->senderId = senderId_; }

    const std::string& getTopic() const { return topic; }

    void setTopic(const std::string& topic_) { this->topic = topic_; }

    const std::string& getKind() const { return kind; }

    void setKind(const std::string& kind_) { this->kind = kind_; }

    const std::unordered_map<std::string,std::string>& getHeader() const { return header; }

    void setHeader(const std::unordered_map<std::string,std::string>& header_) { this->header = header_; }

    const std::string& getBody() const { return body; }

    void setBody(const std::string& body_) { this->body = body_; }

public:
    std::string senderId;
    std::string topic;
    std::string kind;
    std::unordered_map<std::string,std::string> header;
    std::string body;
};

// IpcSubscription
class SYNC_EXPORT IpcSubscription
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getTopic() const { return topic; }

    void setTopic(const std::string& topic_) { this->topic = topic_; }

public:
    std::string id;
    std::string topic;
};

} //namespace apsync
//$generated