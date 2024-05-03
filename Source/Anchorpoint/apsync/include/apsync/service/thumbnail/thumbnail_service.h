#ifndef THUMBNAIL_SERVICE_H
#define THUMBNAIL_SERVICE_H

#include <optional>

#include "apsync/models/file_object.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class ThumbnailServiceImpl;
class SYNC_EXPORT ThumbnailService
{
public:
    ThumbnailService(std::shared_ptr<Api> api);
    Result<void> setThumbnails(const std::optional<ProjectObject>& projectObject, FileObject& file,
                               const std::optional<std::string>& previewImagePath,
                               const std::optional<std::string>& detailImagePath);

    Result<void> copyThumbnails(const std::optional<ProjectObject>& targetProjectObject,
                                const FileObject& sourceFile, FileObject& targetFile);

    Result<void> moveThumbnails(const FileObject& sourceFile, const std::string& targetFolderPath);

    std::vector<std::string> generateThumbnails(const std::optional<ProjectObject>& projectObject,
                                                const std::string& workspaceId,
                                                const std::vector<std::string>& filePaths,
                                                const std::string& outputDirectory,
                                                bool withPreview, bool withDetail);

    Result<void> clearThumbnails(FileObject& file);

    std::optional<std::string> getPreviewImage(const std::optional<FileObject>& file,
                                               const std::string& path) const;
    std::optional<std::string> getDetailImage(const std::optional<FileObject>& file,
                                              const std::string& path) const;

    static std::optional<std::string> computeSimpleFileHash(
        const std::string& filePath,
        const std::optional<std::string>& colorConfigHash = std::nullopt);
    static std::optional<std::string> getCachedPreviewImage(const std::string& filePath,
                                                            std::optional<std::string> fileHash);
    static std::optional<std::string> getCachedDetailImage(const std::string& filePath,
                                                           std::optional<std::string> fileHash);
    static std::optional<std::string> getCachedPreviewImageFromHash(const std::string& fileHash);
    static std::optional<std::string> getCachedDetailImageFromHash(const std::string& fileHash);

    static std::optional<std::string> getAttachedImageForFile(const std::string& fileObjectId,
                                                              const std::string& filePath,
                                                              const std::string& imageName);

    static std::string getMetadataCache();
    static std::string getMetadataCache(const std::string& fileHash);

    static std::optional<std::string> getPreviewImagePath(
        const std::string& filePath,
        const std::optional<std::string>& colorConfigHash = std::nullopt);
    static std::optional<std::string> getDetailImagePath(
        const std::string& filePath,
        const std::optional<std::string>& colorConfigHash = std::nullopt);

private:
    std::shared_ptr<Api> _api;
    std::shared_ptr<ThumbnailServiceImpl> _pimpl;
};
} // namespace apsync

#endif // THUMBNAIL_SERVICE_H
