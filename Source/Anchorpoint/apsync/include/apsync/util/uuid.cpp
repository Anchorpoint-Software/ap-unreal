#include "uuid.h"

#include <shared_mutex>

#include "sole.hpp"

namespace apsync {

std::shared_mutex s_mutex;

std::string s_nil = std::string("00000000-0000-0000-0000-000000000000");

std::string UUID::uuid4()
{
    std::unique_lock<std::shared_mutex> lock(s_mutex);
    return sole::uuid4().str();
}

const std::string& UUID::empty() { return s_nil; }

} // namespace apsync
