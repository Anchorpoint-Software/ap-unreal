#ifndef REMOTE_ATTACHMENT_TYPE_H
#define REMOTE_ATTACHMENT_TYPE_H

#include "apsync/sync_global.h"

namespace apsync {

struct SYNC_EXPORT RemoteAttachmentType {
    RemoteAttachmentType() : _type(previewImage()) {}
    RemoteAttachmentType(const std::string& type) : _type(type) {}
    operator const std::string&() const { return _type; }
    bool operator==(const RemoteAttachmentType& other) const { return _type == other._type; }
    bool operator!=(const RemoteAttachmentType& other) const { return _type != other._type; }

    static RemoteAttachmentType previewImage();
    static RemoteAttachmentType detailImage();

private:
    std::string _type;
};

} // namespace apsync

#endif // REMOTE_ATTACHMENT_TYPE_H
