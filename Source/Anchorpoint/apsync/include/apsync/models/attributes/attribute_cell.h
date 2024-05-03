#ifndef ATTRIBUTE_CELL_H
#define ATTRIBUTE_CELL_H

#include <vector>

#include "apsync/models/attributes/attribute_type.h"
#include "apsync/models/base_document.h"
#include "apsync/util/algorithm.h"

namespace apsync {

struct AttributeTimestamp {
    std::string userId;
    int seconds;
    uint64 date;
    bool edited;
    std::optional<std::string> comment;

    bool operator==(const AttributeTimestamp& other) const
    {
        return userId == other.userId && seconds == other.seconds && date == other.date
            && edited == other.edited && comment == other.comment;
    }
};

struct SYNC_EXPORT AttributeCellValue {
    std::optional<std::vector<std::string>>
        tagIds;                      // Multiple Choice and Single Choice id at index 0.
    std::optional<std::string> text; // Text, Hyperlink
    std::optional<bool> checked;     // Checkbox
    std::optional<uint64> date;      // Date in seconds since epoch
    std::optional<int> rating;       // Rating
    std::optional<std::vector<std::string>> userIds; // Users id
    std::optional<std::vector<AttributeTimestamp>> timestamps;

    bool isEmpty() const
    {
        return !tagIds && !text && !checked && !date && !rating && !timestamps && !userIds;
    }

    AttributeCellValue copy() const
    {
        AttributeCellValue copied = *this;
        copied.timestamps.reset();
        return copied;
    }

    void cleanup()
    {
        if (tagIds && tagIds->empty()) tagIds.reset();
        if (text && text->empty()) text.reset();
        if (checked && !*checked) checked.reset();
        if (date && *date == 0) date.reset();
        if (rating && *rating == 0) rating.reset();
        if (userIds && userIds->empty()) userIds.reset();
        if (timestamps && timestamps->empty()) timestamps.reset();
    }

    bool equals(const AttributeCellValue& other, const AttributeType& type) const
    {
        if (type == AttributeType::checkbox()) return checked == other.checked;
        if (type == AttributeType::date()) return date == other.date;
        if (type == AttributeType::hyperlink()) return text == other.text;
        if (type == AttributeType::text()) return text == other.text;
        if (type == AttributeType::rating()) return rating == other.rating;
        if (type == AttributeType::user()) return compareOptionalContainer(userIds, other.userIds);
        if (type == AttributeType::multipleChoiceTag() || type == AttributeType::singleChoiceTag())
            return compareOptionalContainer(tagIds, other.tagIds);
        if (type == AttributeType::timestamp())
            return compareOptionalContainer(timestamps, other.timestamps);
        return false;
    }
};

struct SYNC_EXPORT AttributeCellContent {
    std::string objectId;
    std::string fieldId;

    AttributeCellValue value;
    AttributeCellValue oldValue;
};

struct SYNC_EXPORT AttributeCell: public BaseDocument {
    std::string databaseId;
    AttributeCellContent content;
    optional<int64> revision;

    bool operator==(const AttributeCell& other) const { return id == other.id; }
};

struct AttributeCellContentChange {
    std::string objectId;
    std::string fieldId;
    AttributeCellValue before;
    AttributeCellValue after;
};

} // namespace apsync

#endif // ATTRIBUTE_CELL_H
