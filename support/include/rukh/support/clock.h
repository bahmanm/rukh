#ifndef RUKH_SUPPORT_CLOCK_H_INCLUDED
#define RUKH_SUPPORT_CLOCK_H_INCLUDED

#include <chrono>
#include <cstdint>

namespace ShashChess
{

    using TimePoint = std::chrono::milliseconds::rep;
    static_assert(sizeof(TimePoint) == sizeof(std::int64_t));

    inline TimePoint
    now()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                   std::chrono::steady_clock::now().time_since_epoch())
            .count();
    }

} // namespace ShashChess

#endif // RUKH_SUPPORT_CLOCK_H_INCLUDED
