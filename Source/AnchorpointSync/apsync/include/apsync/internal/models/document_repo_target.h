#ifndef INCLUDE_MODELS_DOCUMENT_REPO_TARGET
#define INCLUDE_MODELS_DOCUMENT_REPO_TARGET

#include "apsync/sync_global.h"

namespace apsync {
struct SYNC_EXPORT DocumentRepoTarget {
    std::string workspaceId;
    std::string repoName;
};
} // namespace apsync

#endif /* INCLUDE_MODELS_DOCUMENT_REPO_TARGET */
