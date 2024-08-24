#ifndef MENTION_H
#define MENTION_H

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "apsync/models/attributes/database_object.h"
#include "apsync/models/project_object.h"
#include "apsync/sync_global.h"

namespace apsync {

enum class MentionTargetType { Review, Version, Task, TaskComment, UserAttribute };

struct SYNC_EXPORT MentionTargetInterface {
    MentionTargetType type;
};

struct SYNC_EXPORT MentionReviewTarget: MentionTargetInterface {
    MentionReviewTarget() { type = MentionTargetType::Review; }

    std::optional<std::string> filePath;
    std::optional<std::string> pathToPreviewImage;
    std::string reviewId;
};

struct SYNC_EXPORT MentionVersionTarget: MentionTargetInterface {
    MentionVersionTarget() { type = MentionTargetType::Version; }

    std::optional<std::string> filePath;
    std::optional<std::string> pathToPreviewImage;
    std::string versionId;
};

struct SYNC_EXPORT MentionTaskTarget: MentionTargetInterface {
    MentionTaskTarget() { type = MentionTargetType::Task; }

    std::string taskId;
    std::optional<std::string> taskName;
    std::optional<std::string> taskListId;
    std::optional<std::string> taskListPath;
};

struct SYNC_EXPORT MentionTaskCommentTarget: MentionTaskTarget {
    MentionTaskCommentTarget() { type = MentionTargetType::TaskComment; }

    std::string taskJournalId;
};


struct SYNC_EXPORT MentionUserAttributeTarget: MentionTargetInterface {
    MentionUserAttributeTarget() { type = MentionTargetType::UserAttribute; }

    std::optional<DatabaseObject> database;
    std::optional<std::string> pathToObject; // file or folder
    std::optional<std::string> parentPath;   //folder path
    std::optional<std::string> taskId;       // task
    std::optional<std::string> taskName;
    std::optional<std::string> pathToPreviewImage;
};

using MentionTargetInterfaceRef = std::shared_ptr<MentionTargetInterface>;
using MentionReviewTargetRef = std::shared_ptr<MentionReviewTarget>;
using MentionVersionTargetRef = std::shared_ptr<MentionVersionTarget>;
using MentionTaskCommentTargetRef = std::shared_ptr<MentionTaskCommentTarget>;
using MentionTaskTargetRef = std::shared_ptr<MentionTaskTarget>;
using MentionUserAttributeTargetRef = std::shared_ptr<MentionUserAttributeTarget>;

enum class MentionType { Mention, MentionRead };


struct SYNC_EXPORT MentionBase {
    std::string id;
    std::string subjectId;
    std::string workspaceId;
    std::optional<ProjectObject> projectOpt;

    MentionType type;
};

struct SYNC_EXPORT MentionRead: MentionBase {
    MentionRead() { type = MentionType::MentionRead; }
};

struct SYNC_EXPORT Mention: MentionBase {
    Mention() { type = MentionType::Mention; }

    std::vector<std::string> userIds;
    std::vector<MentionTargetInterfaceRef> targets;
};

using MentionBaseRef = std::shared_ptr<MentionBase>;
using MentionReadRef = std::shared_ptr<MentionRead>;
using MentionRef = std::shared_ptr<Mention>;

} // namespace apsync

#endif // MENTION_H
