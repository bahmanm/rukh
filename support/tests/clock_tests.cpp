#include <cstdint>
#include <stdexcept>

#include <rukh/support/clock.h>

namespace
{

    void
    require(
        bool        condition,
        const char* message)
    {
        if (!condition)
            throw std::runtime_error(message);
    }

    void
    test_monotonic_millisecond_clock()
    {
        static_assert(sizeof(ShashChess::TimePoint) == sizeof(std::int64_t));
        const ShashChess::TimePoint before = ShashChess::now();
        const ShashChess::TimePoint after = ShashChess::now();
        require(after >= before, "steady clock moved backwards");
    }

} // namespace

int
main()
{
    test_monotonic_millisecond_clock();
}
