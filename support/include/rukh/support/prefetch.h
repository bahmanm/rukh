#ifndef RUKH_SUPPORT_PREFETCH_H_INCLUDED
#define RUKH_SUPPORT_PREFETCH_H_INCLUDED

#if defined(_MSC_VER) && !defined(NO_PREFETCH)
#include <xmmintrin.h>
#endif

namespace ShashChess
{

    inline void
    prefetch(const void* address)
    {
#ifdef NO_PREFETCH
        (void) address;
#elif defined(_MSC_VER)
        _mm_prefetch(static_cast<const char*>(address), _MM_HINT_T0);
#else
        __builtin_prefetch(address);
#endif
    }

} // namespace ShashChess

#endif // RUKH_SUPPORT_PREFETCH_H_INCLUDED
