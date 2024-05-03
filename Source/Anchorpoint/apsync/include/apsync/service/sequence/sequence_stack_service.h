#ifndef SEQUENCE_STACK_SERVICE_H
#define SEQUENCE_STACK_SERVICE_H

#include "apsync/models/folder_object.h"
#include "apsync/sync_global.h"

namespace apsync {
class Api;
class SequenceStackServiceImpl;
class SYNC_EXPORT SequenceStackService
{
public:
    explicit SequenceStackService(std::shared_ptr<Api> api);

    Result<void> toggleSequenceStack(FolderObject& folderObject, bool enable);

private:
    std::shared_ptr<Api> _api;
    std::shared_ptr<SequenceStackServiceImpl> _pimpl;
};
} // namespace apsync

#endif // SEQUENCE_STACK_SERVICE_H
