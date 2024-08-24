#ifndef UUID_H
#define UUID_H

#include "apsync/sync_global.h"

namespace apsync {
class SYNC_EXPORT UUID
{
public:
    static std::string uuid4();

    static const std::string& empty();
};
} // namespace apsync

#endif // UUID_H
