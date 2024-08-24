//$generated
#pragma once

#include "apsync/sync_global.h"
#include "apsync/api_types.h"

// External includes


// Dependencies
#include "apsync/filters/models.h"

namespace apsync {

template <typename T>
struct ApiMapper;

// Forward declare models
class Icon2;
class AttributeTagData;
class AttributeTimestampEntry;
class TagAttributeColumn;
class TagAttribute;
class TextAttributeColumn;
class TextAttribute;
class RatingAttributeColumn;
class RatingAttribute;
class LinkAttributeColumn;
class LinkAttribute;
class DateAttributeColumn;
class DateAttribute;
class CheckboxAttributeColumn;
class CheckboxAttribute;
class TimestampAttributeColumn;
class TimestampAttribute;
class UserAttributeColumn;
class UserAttribute;
class AttributeWithColumn;
class TagAttributeFilter;
class TextAttributeFilter;
class RatingAttributeFilter;
class LinkAttributeFilter;
class DateAttributeFilter;
class CheckedAttributeFilter;
class UserAttributeFilter;

// Aliases
using AnyAttributeColumn = std::variant< TagAttributeColumn, TextAttributeColumn, RatingAttributeColumn, LinkAttributeColumn, DateAttributeColumn, CheckboxAttributeColumn, TimestampAttributeColumn, UserAttributeColumn>;
using AnyAttribute = std::variant< TagAttribute, TextAttribute, RatingAttribute, LinkAttribute, DateAttribute, CheckboxAttribute, TimestampAttribute, UserAttribute>;
using AnyAttributeFilter = std::variant< TagAttributeFilter, TextAttributeFilter, RatingAttributeFilter, LinkAttributeFilter, DateAttributeFilter, CheckedAttributeFilter, TimestampAttribute, UserAttributeFilter>;

// Enums

// Icon2
class SYNC_EXPORT Icon2
{
public:
    const std::optional<std::string>& getPath() const { return path; }

    void setPath(const std::optional<std::string>& path_) { this->path = path_; }

    const std::optional<std::string>& getColor() const { return color; }

    void setColor(const std::optional<std::string>& color_) { this->color = color_; }

public:
    std::optional<std::string> path;
    std::optional<std::string> color;
};

// AttributeTagData
class SYNC_EXPORT AttributeTagData
{
public:
    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

    const std::string& getColor() const { return color; }

    void setColor(const std::string& color_) { this->color = color_; }

public:
    std::string name;
    std::string color;
};

// AttributeTimestampEntry
class SYNC_EXPORT AttributeTimestampEntry
{
public:
    const std::string& getUserId() const { return userId; }

    void setUserId(const std::string& userId_) { this->userId = userId_; }

    int64 getSeconds() const { return seconds; }

    void setSeconds(int64 seconds_) { this->seconds = seconds_; }

    int64 getDate() const { return date; }

    void setDate(int64 date_) { this->date = date_; }

    bool getEdited() const { return edited; }

    void setEdited(bool edited_) { this->edited = edited_; }

    const std::string& getComment() const { return comment; }

    void setComment(const std::string& comment_) { this->comment = comment_; }

public:
    std::string userId;
    int64 seconds = 0;
    int64 date = 0;
    bool edited = false;
    std::string comment;
};

// TagAttributeColumn
class SYNC_EXPORT TagAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

    const std::unordered_map<std::string, AttributeTagData>& getTags() const { return tags; }

    void setTags(const std::unordered_map<std::string, AttributeTagData>& tags_) { this->tags = tags_; }

    bool getAllowMultipleSelection() const { return allowMultipleSelection; }

    void setAllowMultipleSelection(bool allowMultipleSelection_) { this->allowMultipleSelection = allowMultipleSelection_; }

public:
    std::string id;
    std::string name;
    std::unordered_map<std::string, AttributeTagData> tags;
    bool allowMultipleSelection = false;
};

// TagAttribute
class SYNC_EXPORT TagAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    const std::vector<std::string>& getSelectedTags() const { return selectedTags; }

    void setSelectedTags(const std::vector<std::string>& selectedTags_) { this->selectedTags = selectedTags_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    std::vector<std::string> selectedTags;
};

// TextAttributeColumn
class SYNC_EXPORT TextAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

public:
    std::string id;
    std::string name;
};

// TextAttribute
class SYNC_EXPORT TextAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    const std::string& getText() const { return text; }

    void setText(const std::string& text_) { this->text = text_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    std::string text;
};

// RatingAttributeColumn
class SYNC_EXPORT RatingAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

    int64 getMaximum() const { return maximum; }

    void setMaximum(int64 maximum_) { this->maximum = maximum_; }

public:
    std::string id;
    std::string name;
    int64 maximum = 0;
};

// RatingAttribute
class SYNC_EXPORT RatingAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    int64 getRating() const { return rating; }

    void setRating(int64 rating_) { this->rating = rating_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    int64 rating = 0;
};

// LinkAttributeColumn
class SYNC_EXPORT LinkAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

public:
    std::string id;
    std::string name;
};

// LinkAttribute
class SYNC_EXPORT LinkAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    const std::string& getLink() const { return link; }

    void setLink(const std::string& link_) { this->link = link_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    std::string link;
};

// DateAttributeColumn
class SYNC_EXPORT DateAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

public:
    std::string id;
    std::string name;
};

// DateAttribute
class SYNC_EXPORT DateAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    int64 getDate() const { return date; }

    void setDate(int64 date_) { this->date = date_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    int64 date = 0;
};

// CheckboxAttributeColumn
class SYNC_EXPORT CheckboxAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

public:
    std::string id;
    std::string name;
};

// CheckboxAttribute
class SYNC_EXPORT CheckboxAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    bool getChecked() const { return checked; }

    void setChecked(bool checked_) { this->checked = checked_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    bool checked = false;
};

// TimestampAttributeColumn
class SYNC_EXPORT TimestampAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

public:
    std::string id;
    std::string name;
};

// TimestampAttribute
class SYNC_EXPORT TimestampAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    const std::vector<AttributeTimestampEntry>& getTimestamps() const { return timestamps; }

    void setTimestamps(const std::vector<AttributeTimestampEntry>& timestamps_) { this->timestamps = timestamps_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    std::vector<AttributeTimestampEntry> timestamps;
};

// UserAttributeColumn
class SYNC_EXPORT UserAttributeColumn
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getName() const { return name; }

    void setName(const std::string& name_) { this->name = name_; }

public:
    std::string id;
    std::string name;
};

// UserAttribute
class SYNC_EXPORT UserAttribute
{
public:
    const std::string& getId() const { return id; }

    void setId(const std::string& id_) { this->id = id_; }

    const std::string& getObjectId() const { return objectId; }

    void setObjectId(const std::string& objectId_) { this->objectId = objectId_; }

    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const std::string& getDatabaseId() const { return databaseId; }

    void setDatabaseId(const std::string& databaseId_) { this->databaseId = databaseId_; }

    const std::vector<std::string>& getUsers() const { return users; }

    void setUsers(const std::vector<std::string>& users_) { this->users = users_; }

public:
    std::string id;
    std::string objectId;
    std::string columnId;
    std::string databaseId;
    std::vector<std::string> users;
};

// AttributeWithColumn
class SYNC_EXPORT AttributeWithColumn
{
public:
    const AnyAttributeColumn& getColumn() const { return column; }

    void setColumn(const AnyAttributeColumn& column_) { this->column = column_; }

    const AnyAttribute& getAttribute() const { return attribute; }

    void setAttribute(const AnyAttribute& attribute_) { this->attribute = attribute_; }

public:
    AnyAttributeColumn column;
    AnyAttribute attribute;
};

// TagAttributeFilter
class SYNC_EXPORT TagAttributeFilter
{
public:
    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const AnyStrPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyStrPredicate& predicate_) { this->predicate = predicate_; }

public:
    std::string columnId;
    AnyStrPredicate predicate;
};

// TextAttributeFilter
class SYNC_EXPORT TextAttributeFilter
{
public:
    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const AnyStrPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyStrPredicate& predicate_) { this->predicate = predicate_; }

public:
    std::string columnId;
    AnyStrPredicate predicate;
};

// RatingAttributeFilter
class SYNC_EXPORT RatingAttributeFilter
{
public:
    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const AnyNumPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyNumPredicate& predicate_) { this->predicate = predicate_; }

public:
    std::string columnId;
    AnyNumPredicate predicate;
};

// LinkAttributeFilter
class SYNC_EXPORT LinkAttributeFilter
{
public:
    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const AnyStrPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyStrPredicate& predicate_) { this->predicate = predicate_; }

public:
    std::string columnId;
    AnyStrPredicate predicate;
};

// DateAttributeFilter
class SYNC_EXPORT DateAttributeFilter
{
public:
    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const AnyNumPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyNumPredicate& predicate_) { this->predicate = predicate_; }

public:
    std::string columnId;
    AnyNumPredicate predicate;
};

// CheckedAttributeFilter
class SYNC_EXPORT CheckedAttributeFilter
{
public:
    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const BoolEquals& getPredicate() const { return predicate; }

    void setPredicate(const BoolEquals& predicate_) { this->predicate = predicate_; }

public:
    std::string columnId;
    BoolEquals predicate;
};

// UserAttributeFilter
class SYNC_EXPORT UserAttributeFilter
{
public:
    const std::string& getColumnId() const { return columnId; }

    void setColumnId(const std::string& columnId_) { this->columnId = columnId_; }

    const AnyStrPredicate& getPredicate() const { return predicate; }

    void setPredicate(const AnyStrPredicate& predicate_) { this->predicate = predicate_; }

public:
    std::string columnId;
    AnyStrPredicate predicate;
};

} //namespace apsync
//$generated