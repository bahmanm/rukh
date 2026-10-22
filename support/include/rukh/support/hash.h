#ifndef RUKH_SUPPORT_HASH_H_INCLUDED
#define RUKH_SUPPORT_HASH_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <type_traits>

namespace ShashChess
{

    inline uint64_t
    mul_hi64(
        uint64_t a,
        uint64_t b)
    {
#if defined(__GNUC__) && defined(IS_64BIT)
        __extension__ using uint128 = unsigned __int128;
        return (uint128(a) * uint128(b)) >> 64;
#else
        uint64_t aL = uint32_t(a), aH = a >> 32;
        uint64_t bL = uint32_t(b), bH = b >> 32;
        uint64_t c1 = (aL * bL) >> 32;
        uint64_t c2 = aH * bL + c1;
        uint64_t c3 = aL * bH + uint32_t(c2);
        return aH * bH + (c2 >> 32) + (c3 >> 32);
#endif
    }

    inline std::uint64_t
    hash_bytes(
        const char* data,
        std::size_t size)
    {
        // FNV-1a 64-bit
        const char*   p = data;
        std::uint64_t h = 14695981039346656037ull;
        for (std::size_t i = 0; i < size; ++i)
            h = (h ^ p[i]) * 1099511628211ull;
        return h;
    }

    template <typename T>
    inline std::size_t
    get_raw_data_hash(const T& value)
    {
        // We must have no padding bytes because we're reinterpreting as char
        static_assert(std::has_unique_object_representations<T>());

        return static_cast<std::size_t>(
            hash_bytes(reinterpret_cast<const char*>(&value), sizeof(value)));
    }

    template <typename T>
    inline void
    hash_combine(
        std::size_t& seed,
        const T&     v)
    {
        std::size_t x;
        // For primitive types we avoid using the default hasher, which may be
        // nondeterministic across program invocations
        if constexpr (std::is_integral<T>())
            x = v;
        else
            x = std::hash<T>{}(v);
        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    inline std::uint64_t
    hash_string(const std::string& sv)
    {
        return hash_bytes(sv.data(), sv.size());
    }

} // namespace ShashChess

#endif // RUKH_SUPPORT_HASH_H_INCLUDED
