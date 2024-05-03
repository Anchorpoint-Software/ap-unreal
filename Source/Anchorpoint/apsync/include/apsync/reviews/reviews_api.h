//$generated
#pragma once

#include "apsync/sync_global.h"

#include "models.h"

namespace apsync {

class Api;

class SYNC_EXPORT ReviewsApi
{
public:
    explicit ReviewsApi(std::shared_ptr<Api> api);

    Result<apsync::ReviewEntry> addReview(
        const std::optional<ProjectObject>& project,
        const apsync::FileObject& fileObject,
        const apsync::ReviewEntry& reviewEntry
    );

    Result<std::vector<apsync::ReviewEntry>> listReviews(
        const apsync::FileObject& fileObject
    );

    Result<apsync::ReviewEntry> getReview(
        const std::string& fileObjectId,
        const std::string& reviewId,
        const std::string& workspaceId
    );

    Result<void> updateReview(
        const std::optional<ProjectObject>& project,
        const apsync::ReviewEntry& reviewEntry
    );

    Result<void> deleteReview(
        const std::optional<ProjectObject>& project,
        const std::string& fileObjectId,
        const std::string& reviewId,
        const std::string& workspaceId
    );

private:
    std::shared_ptr<class ReviewsApiImpl> _impl;
};

} // namespace apsync
//$generated