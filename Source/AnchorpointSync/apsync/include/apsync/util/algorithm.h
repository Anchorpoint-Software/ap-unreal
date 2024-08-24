#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <optional>

template<typename CONTAINER>
static bool compareOptionalContainer(const std::optional<CONTAINER>& a,
                                     const std::optional<CONTAINER>& b)
{
    if (!a.has_value() && !b.has_value()) return true;
    if (a.has_value() && b.has_value()) {
        if (a->size() != b->size()) return false;
        return std::equal(a->begin(), a->end(), b->begin());
    }
    return false;
}

#endif // ALGORITHM_H
