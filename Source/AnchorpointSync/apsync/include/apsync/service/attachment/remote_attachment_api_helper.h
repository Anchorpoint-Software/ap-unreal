#ifndef ATTACHMENT_API_HELPER_H
#define ATTACHMENT_API_HELPER_H

#include "apsync/sync_global.h"
#include <vector>

namespace apsync {

class ObjectApi;
class RemoteAttachmentApi;
class SYNC_EXPORT AttachmentApiHelper
{
public:
    AttachmentApiHelper(std::shared_ptr<ObjectApi> objectApi,
                        std::shared_ptr<RemoteAttachmentApi> attachmentApi);

    Result<std::optional<std::string>> attachThumbnail(const std::string& filePath,
                                                       const std::string& localFile,
                                                       bool isDetail);

    Result<std::vector<std::optional<std::string>>> attachThumbnails(
        const std::string& filePath,
        const std::string& localPreviewFile,
        const std::string& localDetailFile);

    Result<std::optional<std::string>> getThumbnail(const std::string& filePath, bool isDetail);

private:
    std::shared_ptr<ObjectApi> _objectApi;
    std::shared_ptr<RemoteAttachmentApi> _attachmentApi;
};

} // namespace apsync

#endif // ATTACHMENT_API_HELPER_H
