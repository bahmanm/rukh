#include <cstdint>
#include <stdexcept>

#include <rukh/support/shared_memory.h>

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
    test_shared_constant()
    {
        constexpr std::uint64_t                                   value = 0x524b5348504d454d;
        const ShashChess::SystemWideSharedConstant<std::uint64_t> shared(value, 1);

        require(*shared == value, "shared constant did not preserve its value");
        require(
            shared.get_status() !=
                ShashChess::SystemWideSharedConstantAllocationStatus::NoAllocation,
            "shared constant failed to allocate shared or local storage");
    }

} // namespace

int
main()
{
    test_shared_constant();
}
