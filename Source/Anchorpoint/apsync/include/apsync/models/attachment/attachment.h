#ifndef JOURNAL_ATTACHMENT_H
#define JOURNAL_ATTACHMENT_H

#include <vector>

#include "apsync/models/base_document.h"
#include "apsync/models/base_object.h"
#include "apsync/models/icon/icon_model.h"
#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT FavoriteEntry {
    uint64_t favoritedAt;
    std::string favoritedBy;
};

struct SYNC_EXPORT AttachmentSubEntry {
    uint64_t modifiedAt;
    uint64_t createdAt;
    std::string fileId;

    std::optional<std::string> comment;
    std::optional<std::string> pathToFile;
    std::optional<std::string> pathToPreviewThumbnail;
};

struct SYNC_EXPORT AttachmentVersionSubEntry: public AttachmentSubEntry {
    std::string versionEntryId;
    std::optional<std::string> userId;
    int versionNumber;
};

struct SYNC_EXPORT AttachmentReviewSubEntry: public AttachmentSubEntry {
    std::string reviewId;
    std::string userId;

    std::string filename = "";

    std::optional<std::string> drawingData;
    bool drawingIsEdited;
    std::optional<int> frame;
    std::optional<int> maxFrame;
    std::optional<int> versionNumber;
};

struct SYNC_EXPORT BaseAttachment {
    std::string id;
    uint64_t createdAt;
    std::optional<FavoriteEntry> favoriteEntry;
    enum class Type { FILE, FOLDER, EXTFILE, EXTFOLDER } type;
};

struct SYNC_EXPORT FileAttachment: public BaseAttachment {
    FileAttachment() { type = BaseAttachment::Type::FILE; }

    std::string objectId;
    std::optional<std::string> path;
    std::optional<std::string> pathToPreviewThumbnail;

    std::optional<std::string> baseName; // Version or Sequence Stack baseName, e.g. tower
    std::optional<std::string>
        baseNameFormatExtension;               // Version or Sequence Stack extension, e.g. _v###
    std::optional<std::string> baseNameSuffix; // Version or Sequence Stack suffix, e.g. .blend

    std::optional<std::string> versionObjectId;
    std::optional<std::string> latestVersionEntryId;

    bool isSequenceStack = false;

    std::vector<AttachmentReviewSubEntry> reviewSubEntries;   // sorted by date
    std::vector<AttachmentVersionSubEntry> versionSubEntries; // sorted by date
};

struct SYNC_EXPORT FolderAttachment: public BaseAttachment {
    FolderAttachment() { type = BaseAttachment::Type::FOLDER; }
    std::string objectId;
    std::optional<std::string> path;
    std::optional<Icon> icon;
};

struct SYNC_EXPORT ExternalFileAttachment: public BaseAttachment {

    ExternalFileAttachment() { type = BaseAttachment::Type::EXTFILE; }
    std::string path;
    std::string originalFilename;
    std::string objectId;
    std::optional<std::string> pathToPreviewThumbnail;

    std::vector<AttachmentReviewSubEntry> reviewSubEntries; // sorted by date
};

struct SYNC_EXPORT ExternalFolderAttachment: public BaseAttachment {
    ExternalFolderAttachment() { type = BaseAttachment::Type::EXTFOLDER; }
    std::optional<std::string> path;
};

using AttachmentRef = std::shared_ptr<BaseAttachment>;
using FileAttachmentRef = std::shared_ptr<FileAttachment>;
using FolderAttachmentRef = std::shared_ptr<FolderAttachment>;
using ExternalFolderAttachmentRef = std::shared_ptr<ExternalFolderAttachment>;
using ExternalFileAttachmentRef = std::shared_ptr<ExternalFileAttachment>;

struct SYNC_EXPORT AttachmentDocumentContent {
    ObjectType type;

    std::optional<std::string> filename;             // for attached external file in .ap folder
    std::optional<std::string> relativeExternalPath; // for external folder
    std::optional<std::string>
        linkedId; // for project internal file or folder - or when there is no project
    std::optional<FavoriteEntry> favoriteEntry;
};

struct SYNC_EXPORT AttachmentDocument: public BaseDocument {
    AttachmentDocumentContent content;

    std::string objectId;
};
} // namespace apsync

#endif // JOURNAL_ATTACHMENT_H
