#ifndef ATTACHMENT_API_H
#define ATTACHMENT_API_H

#include <functional>
#include <unordered_map>

#include "apsync/models/attachment/remote_attachment_type.h"
#include "apsync/models/base_object.h"
#include "apsync/models/file_object.h"
#include "apsync/models/folder_object.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class RemoteAttachmentApiImpl;
class SYNC_EXPORT RemoteAttachmentApi
{
public:
    explicit RemoteAttachmentApi(std::shared_ptr<Api> api);

    Result<std::optional<std::string>> attach(const BaseObject& object,
                                              const std::string& localFile,
                                              const std::string& remotePath);

    Result<std::vector<std::optional<std::string>>> attach(
        const BaseObject& object, const std::vector<std::string>& localFiles,
        const std::vector<std::string>& remotePaths);

    Result<std::optional<std::string>> attachThumbnail(FileObject fileObject,
                                                       const std::string& localFile, bool isDetail);

    Result<std::vector<std::optional<std::string>>> attachThumbnails(
        FileObject fileObject,
        const std::string& localPreviewFile,
        const std::string& localDetailFile);

    Result<std::optional<std::string>> get(const BaseObject& object, const std::string& remotePath);

    Result<std::unordered_map<std::string, std::string>> get(
        const std::vector<BaseObject>& objects, const std::string& remotePath,
        std::function<bool(const std::unordered_map<std::string, std::string>& cachedPaths)>
            callback = nullptr);

    Result<std::optional<std::string>> getThumbnail(const FileObject& fileObject,
                                                    bool isDetail) const;

    Result<std::unordered_map<std::string, std::string>> getThumbnails(
        const std::vector<FileObject>& fileObjects, bool isDetail,
        std::function<bool(const std::unordered_map<std::string, std::string>& cachedPaths)>
            callback = nullptr) const;

    Result<void> clearThumbnails(const FileObject& fileObject);

    Result<BaseObject> copy(const BaseObject& sourceObject, const BaseObject& targetObject);

private:
    std::shared_ptr<RemoteAttachmentApiImpl> _pimpl;
    std::shared_ptr<Api> _api;
};
} // namespace apsync

#endif // ATTACHMENT_API_H
