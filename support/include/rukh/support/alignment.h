#ifndef RUKH_SUPPORT_ALIGNMENT_H_INCLUDED
#define RUKH_SUPPORT_ALIGNMENT_H_INCLUDED

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace ShashChess
{

    inline void
    assert_aligned(
        const void* ptr,
        std::size_t alignment)
    {
        assert(reinterpret_cast<std::uintptr_t>(ptr) % alignment == 0);
    }

} // namespace ShashChess

#endif // RUKH_SUPPORT_ALIGNMENT_H_INCLUDED
