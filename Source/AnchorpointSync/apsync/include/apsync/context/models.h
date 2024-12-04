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
class Context;

// Aliases

// Enums

// Context
class SYNC_EXPORT Context
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

public:
    std::string id;
};

} //namespace apsync
//$generated