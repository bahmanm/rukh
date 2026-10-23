#ifndef RUKH_SUPPORT_ALGORITHM_H_INCLUDED
#define RUKH_SUPPORT_ALGORITHM_H_INCLUDED

#include <algorithm>
#include <vector>

namespace ShashChess::utility
{

    template <typename T, typename Predicate>
    void
    move_to_front(std::vector<T>& values, Predicate predicate)
    {
        const auto it = std::find_if(values.begin(), values.end(), predicate);
        if (it != values.end())
            std::rotate(values.begin(), it, it + 1);
    }

} // namespace ShashChess::utility

#endif // RUKH_SUPPORT_ALGORITHM_H_INCLUDED
