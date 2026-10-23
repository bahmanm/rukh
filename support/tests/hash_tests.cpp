#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

#include <rukh/support/hash.h>

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
    test_hash_string()
    {
        require(
            ShashChess::hash_string("") == 14695981039346656037ull,
            "empty string hash differs from FNV-1a offset basis");
        require(
            ShashChess::hash_string("hello") == 0xa430d84680aabd0bull,
            "string hash differs from the FNV-1a reference value");
    }

    void
    test_hash_helpers()
    {
        const std::uint32_t value = 0x12345678;
        require(
            ShashChess::get_raw_data_hash(value) ==
                ShashChess::hash_bytes(reinterpret_cast<const char*>(&value), sizeof(value)),
            "raw data hash differs from byte hash");

        std::size_t seed = 0;
        ShashChess::hash_combine(seed, 7);
        require(seed == 0x9e3779c0, "integral hash combination changed");

        require(
            ShashChess::mul_hi64(UINT64_MAX, UINT64_MAX) == UINT64_MAX - 1,
            "high half of 64-bit multiplication is incorrect");
    }

} // namespace

int
main()
{
    test_hash_string();
    test_hash_helpers();
}
