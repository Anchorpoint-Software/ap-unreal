#ifndef REVIEW_ENTRY_H
#define REVIEW_ENTRY_H

#include <vector>

#include "apsync/models/base_document.h"

namespace apsync {

struct SYNC_EXPORT ReviewSubEntry {
    std::string drawingData;
    bool drawingIsEdited;
    int frame;
};

struct SYNC_EXPORT ReviewEntryContent {
    std::optional<std::string> comment;
    std::optional<std::string> drawingData;
    uint64 date;
    std::string userId;
    bool commentIsEdited;
    bool drawingIsEdited;
    std::optional<std::string> parentReviewId; //indicates isReply
    std::optional<int> frame;
    std::vector<ReviewSubEntry> subEntries;
};

struct SYNC_EXPORT ReviewEntry: public BaseDocument {
    ReviewEntryContent content;
    std::string objectId;
};

} // namespace apsync

#endif // REVIEW_ENTRY_H
