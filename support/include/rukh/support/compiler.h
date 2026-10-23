#ifndef RUKH_SUPPORT_COMPILER_H_INCLUDED
#define RUKH_SUPPORT_COMPILER_H_INCLUDED

#define RUKH_SUPPORT_ASSUME(cond)

#if defined(_MSC_VER)
#define RUKH_SUPPORT_ALWAYS_INLINE __forceinline
#elif defined(__clang__)
#define RUKH_SUPPORT_ALWAYS_INLINE __attribute__((always_inline))
#else
#define RUKH_SUPPORT_ALWAYS_INLINE
#endif

#endif // RUKH_SUPPORT_COMPILER_H_INCLUDED
