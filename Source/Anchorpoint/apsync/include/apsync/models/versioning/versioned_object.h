#ifndef VERSIONED_OBJECT_H
#define VERSIONED_OBJECT_H

#include <vector>

#include "apsync/models/base_object.h"
#include "apsync/models/versioning/version_entry.h"

namespace apsync {
enum IncrementStyle {
    None = 0,
    Single = 1 << 0, // 0, 1, 2, ..., 10
    Double = 1 << 1, // 00, 01, ..., 42
    Three = 1 << 2,  // 000, 001, ..., 042
    Four = 1 << 3,   // 0000, 0001, ..., 0042
    Five = 1 << 4,   // 00000, 00001, ..., 00042
    v = 1 << 5,      // v0, v000, v0000
    us = 1 << 6      // _0, _000, _0000
};

inline IncrementStyle operator|(IncrementStyle a, IncrementStyle b)
{
    return static_cast<IncrementStyle>(static_cast<int>(a) | static_cast<int>(b));
}

struct SYNC_EXPORT VersionedObjectProps {
    IncrementStyle incStyle;
    std::string baseName;
    std::optional<std::string> publishedId;
};

struct SYNC_EXPORT VersionedObject: public BaseObject {
    // resource is: folderId/basename.extension
    VersionedObjectProps properties;

    inline std::string getFolderId() const
    {
        int p = resource.find("/");
        return resource.substr(0, p);
    }

    bool isFile;
    std::string absolutePath;
    std::vector<VersionEntry> versions; // order: first version to last version
};

} // namespace apsync

#endif // VERSIONED_OBJECT_H
