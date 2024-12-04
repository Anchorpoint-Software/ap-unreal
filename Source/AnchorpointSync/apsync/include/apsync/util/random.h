#ifndef ANCHORPOINT_RANDOM_H
#define ANCHORPOINT_RANDOM_H

#include <random>
#include <sstream>

namespace apsync {
inline int randomNumber(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(min, max);
    return uni(rng);
}

inline std::string randomId32()
{
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; ++i) ss << randomNumber(0, 15);
    return ss.str();
}
} // namespace apsync

#endif // ANCHORPOINT_RANDOM_H
