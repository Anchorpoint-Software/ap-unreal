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
class QuotaValue;

// Aliases

// Enums

// QuotaValue
class SYNC_EXPORT QuotaValue
{
public:
    int64 getValue() const { return value; }

    void setValue(int64 value_) { this->value = value_; }

    int64 getLimit() const { return limit; }

    void setLimit(int64 limit_) { this->limit = limit_; }

public:
    int64 value = 0;
    int64 limit = 0;
};

} //namespace apsync
//$generated