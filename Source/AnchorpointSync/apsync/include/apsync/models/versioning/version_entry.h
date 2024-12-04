#ifndef VERSION_ENTRY_H
#define VERSION_ENTRY_H

#include "apsync/models/base_object.h"

namespace apsync {

struct SYNC_EXPORT VersionEntryContent {
    std::string objectId; // file or folder
    int versionNumber;

    std::optional<std::string> comment;
    std::optional<std::string> ancestorId;
    std::optional<std::string> author;
    std::optional<uint64> date;
    std::optional<bool> disabled;
    std::optional<bool> isPublished;

    bool commentEdited = false; // put that here bc a deleted comment entry is marked edited as well
};

struct SYNC_EXPORT VersionEntry: public BaseObject {
    // resource is version object id
    std::string absolutePath;
    int reviewCount = 0;
    VersionEntryContent content;
};
} // namespace apsync

#endif // VERSION_ENTRY_H
