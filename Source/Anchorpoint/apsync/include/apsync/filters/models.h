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
class StrEquals;
class StrContains;
class NumEquals;
class NumGreater;
class NumLess;
class NumRange;
class BoolEquals;
class PathMatches;
class NameFilter;
class DateFilterNew;
class PathFilter;

// Aliases
using AnyStrPredicate = std::variant< StrEquals, StrContains>;
using AnyNumPredicate = std::variant< NumEquals, NumGreater, NumLess, NumRange>;

// Enums

// StrEquals
class SYNC_EXPORT StrEquals
{
public:
    const std::vector<std::string>& getValues() const { return values; }

    void setValues(const std::vector<std::string>& values_) { this->values = values_; }

    bool getNegate() const { return negate; }

    void setNegate(bool negate_) { this->negate = negate_; }

public:
    std::vector<std::string> values;
    bool negate = false;
};

// StrContains
class SYNC_EXPORT StrContains
{
public:
    const std::vector<std::string>& getValues() const { return values; }

    void setValues(const std::vector<std::string>& values_) { this->values = values_; }

    bool getNegate() const { return negate; }

    void setNegate(bool negate_) { this->negate = negate_; }

public:
    std::vector<std::string> values;
    bool negate = false;
};

// NumEquals
class SYNC_EXPORT NumEquals
{
public:
    int64 getValue() const { return value; }

    void setValue(int64 value_) { this->value = value_; }

    bool getNegate() const { return negate; }

    void setNegate(bool negate_) { this->negate = negate_; }

public:
    int64 value = 0;
    bool negate = false;
};

// NumGreater
class SYNC_EXPORT NumGreater
{
public:
    int64 getValue() const { return value; }

    void setValue(int64 value_) { this->value = value_; }

public:
    int64 value = 0;
};

// NumLess
class SYNC_EXPORT NumLess
{
public:
    int64 getValue() const { return value; }

    void setValue(int64 value_) { this->value = value_; }

public:
    int64 value = 0;
};

// NumRange
class SYNC_EXPORT NumRange
{
public:
    int64 getFrom() const { return from; }

    void setFrom(int64 from_) { this->from = from_; }

    int64 getTo() const { return to; }

    void setTo(int64 to_) { this->to = to_; }

    bool getNegate() const { return negate; }

    void setNegate(bool negate_) { this->negate = negate_; }

public:
    int64 from = 0;
    int64 to = 0;
    bool negate = false;
};

// BoolEquals
class SYNC_EXPORT BoolEquals
{
public:
    bool getValue() const { return value; }

    void setValue(bool value_) { this->value = value_; }

public:
    bool value = false;
};

// PathMatches
class SYNC_EXPORT PathMatches
{
public:
    const std::string& getPath() const { return path; }

    void setPath(const std::string& path_) { this->path = path_; }

    bool getWithSubfolders() const { return withSubfolders; }

    void setWithSubfolders(bool withSubfolders_) { this->withSubfolders = withSubfolders_; }

public:
    std::string path;
    bool withSubfolders = false;
};

// NameFilter
class SYNC_EXPORT NameFilter
{
public:
    const AnyStrPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyStrPredicate& predicate_) { this->predicate = predicate_; }

public:
    AnyStrPredicate predicate;
};

// DateFilterNew
class SYNC_EXPORT DateFilterNew
{
public:
    const AnyNumPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyNumPredicate& predicate_) { this->predicate = predicate_; }

public:
    AnyNumPredicate predicate;
};

// PathFilter
class SYNC_EXPORT PathFilter
{
public:
    const std::vector<PathMatches>& getRules() const { return rules; }

    void setRules(const std::vector<PathMatches>& rules_) { this->rules = rules_; }

public:
    std::vector<PathMatches> rules;
};

} //namespace apsync
//$generated