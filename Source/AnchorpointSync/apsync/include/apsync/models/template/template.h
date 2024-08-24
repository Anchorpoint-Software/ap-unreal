#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "apsync/sync_global.h"


namespace apsync {
enum class TemplateType { Project, Folder, File };
struct SYNC_EXPORT TemplateInfo {
    TemplateType type;
};
} // namespace apsync

#endif // TEMPLATE_H
