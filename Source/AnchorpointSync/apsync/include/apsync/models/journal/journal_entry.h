#ifndef JOURNAL_ENTRY_H
#define JOURNAL_ENTRY_H

#include <memory>

#include "apsync/models/attachment/attachment.h"
#include "apsync/models/attributes/attribute_cell.h"
#include "apsync/models/attributes/attribute_field.h"
#include "apsync/models/attributes/database_object.h"
#include "apsync/models/attributes/database_type.h"
#include "apsync/models/base_document.h"
#include "apsync/models/icon/icon_model.h"
#include "apsync/models/lock/lock_object.h"
#include "journal_type.h"

namespace apsync {
enum class PATCH_INTERVAL { DEFAULT, TIMELINE, TASK };

struct BaseJournalEntry;
using BaseJournalEntryRef = shared_ptr<BaseJournalEntry>;

struct SYNC_EXPORT JournalPage {
    std::vector<BaseJournalEntryRef> entries;
    uint64 nextTimestamp = 0;
    bool hasMoreEntries = false;
};

//--------------------------- request task journal types -----------------------------

struct SYNC_EXPORT BaseRequestAddJournalEntry {
    string userId;
    JournalType type;
    PATCH_INTERVAL patchInterval = PATCH_INTERVAL::DEFAULT;
};
using BaseRequestAddJournalEntryRef = shared_ptr<BaseRequestAddJournalEntry>;

struct SYNC_EXPORT RequestAddTaskCommentEntry: BaseRequestAddJournalEntry {
    RequestAddTaskCommentEntry() { type = JournalType::taskComment(); }
    optional<string> parentId;
    string comment;
};
using RequestAddTaskCommentEntryRef = shared_ptr<RequestAddTaskCommentEntry>;

struct SYNC_EXPORT RequestAddTaskCheckedEntry: BaseRequestAddJournalEntry {
    RequestAddTaskCheckedEntry() { type = JournalType::taskChecked(); }
    bool checked;
};
using RequestAddTaskCheckedEntryRef = shared_ptr<RequestAddTaskCheckedEntry>;

struct SYNC_EXPORT RequestAddAttachmentEntry: BaseRequestAddJournalEntry {
    RequestAddAttachmentEntry() { type = JournalType::taskAttachment(); }

    vector<AttachmentDocument> attachments;
};
using RequestAddAttachmentEntryRef = shared_ptr<RequestAddAttachmentEntry>;

struct SYNC_EXPORT RequestDeleteAttachmentEntry: BaseRequestAddJournalEntry {
    RequestDeleteAttachmentEntry() { type = JournalType::taskAttachmentDeleted(); }

    vector<AttachmentDocument> attachments;
};
using RequestDeleteAttachmentEntryRef = shared_ptr<RequestDeleteAttachmentEntry>;

//--------------------------- request history types ---------------------------

struct SYNC_EXPORT BaseRequestAddJournalHistoryEntry: BaseRequestAddJournalEntry {
    BaseRequestAddJournalHistoryEntry() { patchInterval = PATCH_INTERVAL::TIMELINE; }
};

struct SYNC_EXPORT RequestAddHistoryTaskCheckedEntry: BaseRequestAddJournalHistoryEntry {
    RequestAddHistoryTaskCheckedEntry() { type = JournalType::historyTaskChecked(); }

    string taskId;
    bool checked;
};
using RequestAddHistoryTaskCheckedEntryRef = shared_ptr<RequestAddHistoryTaskCheckedEntry>;

struct SYNC_EXPORT RequestAddHistoryReviewAddedEntry: BaseRequestAddJournalHistoryEntry {
    RequestAddHistoryReviewAddedEntry() { type = JournalType::historyReviewAdded(); }

    string reviewId;
    string fileId;
    optional<string> comment;
};
using RequestAddHistoryReviewAddedEntryRef = shared_ptr<RequestAddHistoryReviewAddedEntry>;

struct SYNC_EXPORT RequestAddHistoryAttributesChangedEntry: BaseRequestAddJournalHistoryEntry {
    RequestAddHistoryAttributesChangedEntry() { type = JournalType::historyAttributesChanged(); }

    vector<AttributeCellContentChange> changes;
    AttributeType fieldType;
    string databaseId;
    DatabaseType databaseType;
};
using RequestAddHistoryAttributesChangedEntryRef =
    shared_ptr<RequestAddHistoryAttributesChangedEntry>;

struct SYNC_EXPORT RequestAddHistoryCustomEntry: BaseRequestAddJournalHistoryEntry {
    RequestAddHistoryCustomEntry() { type = JournalType::historyCustom(); }

    string text;
    optional<string> objectId;
    optional<ObjectType> objectType;
};
using RequestAddHistoryCustomEntryRef = shared_ptr<RequestAddHistoryCustomEntry>;

struct SYNC_EXPORT RequestAddHistoryTaskAttachmentEntry: BaseRequestAddJournalHistoryEntry {
    RequestAddHistoryTaskAttachmentEntry() { type = JournalType::historyTaskAttachment(); }

    string taskId;
    string attachedToJournalEntryId;
    vector<AttachmentDocumentContent> attachments;
};
using RequestAddHistoryTaskAttachmentEntryRef = shared_ptr<RequestAddHistoryTaskAttachmentEntry>;

struct SYNC_EXPORT RequestAddHistoryLocksEntry: BaseRequestAddJournalHistoryEntry {
    RequestAddHistoryLocksEntry() { type = JournalType::historyLocks(); }

    vector<LockObjectChange> changes;
    bool locked;
};
using RequestAddHistoryLocksForceReleasedEntryRef = shared_ptr<RequestAddHistoryLocksEntry>;

//------------------------------- task journal types ----------------------------------

struct SYNC_EXPORT BaseJournalEntry: BaseDocument {
    string userId;
    string objectId;
    JournalType type;
    optional<Icon> icon;
};

struct SYNC_EXPORT TaskCommentEntry: BaseJournalEntry {
    TaskCommentEntry() { type = JournalType::taskComment(); }
    optional<string> parentId;
    string comment;
    bool isEdited = false;
    optional<uint64> editedAt;
};
using TaskCommentEntryRef = shared_ptr<TaskCommentEntry>;

struct SYNC_EXPORT TaskCheckedEntry: BaseJournalEntry {
    TaskCheckedEntry() { type = JournalType::taskChecked(); }
    bool checked;
};
using TaskCheckedEntryRef = shared_ptr<TaskCheckedEntry>;

struct SYNC_EXPORT TaskAttachmentEntry: BaseJournalEntry {
    TaskAttachmentEntry() { type = JournalType::taskAttachment(); }
    vector<FileAttachmentRef> fileAttachments;
    vector<FolderAttachmentRef> folderAttachments;
    vector<ExternalFileAttachmentRef> extFileAttachments;
    vector<ExternalFolderAttachmentRef> extFolderAttachments;
};
using TaskAttachmentEntryRef = shared_ptr<TaskAttachmentEntry>;

//------------------------------- history types ----------------------------------

struct SYNC_EXPORT BaseHistoryTaskEntry: BaseJournalEntry {

    string attachedToTaskId;

    //resolved
    optional<string> taskName;
    optional<string> tasklistId;
    optional<string> path; //folder or page path
    optional<string> tasklistName;
};
using BaseHistoryTaskEntryRef = shared_ptr<BaseHistoryTaskEntry>;

struct SYNC_EXPORT HistoryTaskCommentEntry: BaseHistoryTaskEntry {
    HistoryTaskCommentEntry() { type = JournalType::historyTaskComment(); }

    string attachedToJournalEntryId;
    string comment;
    bool isEdited = false;
    optional<uint64> editedAt;
};
using HistoryTaskCommentEntryRef = shared_ptr<HistoryTaskCommentEntry>;

struct SYNC_EXPORT HistoryTaskCheckedEntry: BaseHistoryTaskEntry {
    HistoryTaskCheckedEntry() { type = JournalType::historyTaskChecked(); }

    bool checked;
};
using HistoryTaskCheckedEntryRef = shared_ptr<HistoryTaskCheckedEntry>;

struct SYNC_EXPORT HistoryTaskAttachmentEntry: BaseHistoryTaskEntry {
    HistoryTaskAttachmentEntry() { type = JournalType::historyTaskAttachment(); }
    string attachedToJournalEntryId;
    vector<FileAttachmentRef> fileAttachments;
    vector<FolderAttachmentRef> folderAttachments;
    vector<ExternalFileAttachmentRef> extFileAttachments;
    vector<ExternalFolderAttachmentRef> extFolderAttachments;
};
using HistoryTaskAttachmentEntryRef = shared_ptr<HistoryTaskAttachmentEntry>;

struct SYNC_EXPORT HistoryReviewAddedEntry: BaseJournalEntry {
    HistoryReviewAddedEntry() { type = JournalType::historyReviewAdded(); }

    string attachedToReviewId;
    string attachedToFileId;
    optional<string> comment;

    //resolved
    optional<bool> isFirstReview;
    optional<int> firstFrame;
    optional<int> maxFrame;
    optional<string> pathToFile;
    optional<string> pathToPreviewThumbnail;
    optional<string> drawingData;
    optional<bool> drawingEdited;
    optional<bool> commentEdited;
};
using HistoryReviewAddedEntryRef = shared_ptr<HistoryReviewAddedEntry>;

struct SYNC_EXPORT HistoryAttributesChangedEntry: BaseJournalEntry {
    HistoryAttributesChangedEntry() { type = JournalType::historyAttributesChanged(); }

    struct AttributeChange {
        AttributeCellValue before;
        AttributeCellValue after;
        optional<string> path; // path to file or folder
        optional<Icon> icon;

        // Task
        // Only valid if databaseType == DatabaseType::Task
        optional<string> attachedToTaskId;
        optional<string> taskName;
        optional<bool> taskChecked;
        optional<string> tasklistId;
        optional<string> taskListPath; //folder or page path
        optional<string> tasklistName;
    };

    optional<AttributeField> field;
    AttributeType fieldType;
    optional<DatabaseObject> database;
    DatabaseType databaseType;
    optional<string> path; //folder or page path
    vector<AttributeChange> attributeChanges;
};
using HistoryAttributesChangedEntryRef = shared_ptr<HistoryAttributesChangedEntry>;

struct SYNC_EXPORT HistoryCustomEntry: BaseJournalEntry {
    HistoryCustomEntry() { type = JournalType::historyCustom(); }

    string text;
    optional<string> path;

    optional<ObjectType> objectType;
    optional<string> objectId;
};
using HistoryCustomEntryRef = shared_ptr<HistoryCustomEntry>;

struct SYNC_EXPORT HistoryLockEntry: BaseJournalEntry {
    HistoryLockEntry() { type = JournalType::historyLocks(); }

    struct LockEntry {
        string path;
        optional<string> previewImage;
        string ownerId;
    };

    vector<LockEntry> locks;
    bool locked;
};
using HistoryLockEntryRef = shared_ptr<HistoryLockEntry>;

//------------------------------- JournalEntryContent ----------------------------------

struct SYNC_EXPORT JournalEntryContent {
    string userId;
    JournalType type;

    optional<string> attachedToDocumentId; //e.g. journalEntryId or reviewEntryId
    optional<string> attachedToObjectId;   //e.g. taskId on timeline entry

    optional<string> parentId;
    optional<string> comment;
    bool isEdited = false;
    optional<uint64> editedAt;
    optional<bool> checked;

    // Attributes
    optional<vector<AttributeCellContentChange>> attributeChanges;
    optional<string> databaseType;
    optional<string> fieldType;

    // Custom (e.g. Python)
    optional<ObjectType> objectType;

    // Files & Folders on Tasks
    optional<vector<string>> attachmentIds;

    // Files & Folders on History (Duplicated)
    optional<vector<AttachmentDocumentContent>> attachments;

    // Locks
    optional<vector<LockObjectChange>> lockChanges;
    optional<bool> locked;
};

struct SYNC_EXPORT InternalJournalEntry: public BaseDocument {
    JournalEntryContent content;
    string objectId;

    const bool operator<(const InternalJournalEntry& rhs) const
    {
        return createdAt < rhs.createdAt;
    }
};
} // namespace apsync

#endif // JOURNAL_ENTRY_H
