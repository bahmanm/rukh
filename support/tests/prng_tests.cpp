#include <cstdint>
#include <stdexcept>

#include <rukh/support/prng.h>

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
    test_sequence()
    {
        ShashChess::PRNG first(1);
        ShashChess::PRNG second(1);

        require(
            first.rand<std::uint64_t>() == 0x47e4ce4b896cdd1dULL,
            "first xorshift64* output changed");
        require(
            second.rand<std::uint64_t>() == 0x47e4ce4b896cdd1dULL,
            "equal seeds did not produce equal output");
        require(
            first.rand<std::uint64_t>() == second.rand<std::uint64_t>(),
            "equal seeds did not remain in sync");
    }

    void
    test_sparse_sequence()
    {
        ShashChess::PRNG random(1);
        require(
            random.sparse_rand<std::uint64_t>() == 0x01c0040880600515ULL,
            "sparse xorshift64* output changed");
    }

} // namespace

int
main()
{
    test_sequence();
    test_sparse_sequence();
}
