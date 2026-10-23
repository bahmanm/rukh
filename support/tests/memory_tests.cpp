#include <cstddef>
#include <cstdint>
#include <new>
#include <stdexcept>

#include <rukh/support/memory.h>

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

    struct alignas(64) Payload
    {
            explicit Payload(int v) : value(v)
            {}

            int value;
    };

    struct Lifetime
    {
            Lifetime()
            {
                ++constructed;
            }

            ~Lifetime()
            {
                ++destroyed;
            }

            static inline int constructed = 0;
            static inline int destroyed = 0;
    };

    void
    test_raw_allocators()
    {
        void* aligned = ShashChess::std_aligned_alloc(64, 128);
        require(aligned != nullptr, "aligned allocation failed");
        require(
            reinterpret_cast<std::uintptr_t>(aligned) % 64 == 0,
            "aligned allocation returned a misaligned address");
        ShashChess::std_aligned_free(aligned);

        void* large_page = ShashChess::aligned_large_pages_alloc(sizeof(Payload));
        require(large_page != nullptr, "large-page allocation fallback failed");
        require(
            reinterpret_cast<std::uintptr_t>(large_page) % 4096 == 0,
            "large-page allocation returned an insufficiently aligned address");
        ShashChess::aligned_large_pages_free(large_page);
    }

    void
    test_allocation_failure()
    {
        const auto fail_to_allocate = [](std::size_t) -> void* { return nullptr; };

        bool object_allocation_failed = false;
        try
        {
            (void)ShashChess::memory_allocator<Payload>(fail_to_allocate, 1);
        }
        catch (const std::bad_alloc&)
        {
            object_allocation_failed = true;
        }
        require(object_allocation_failed, "failed object allocation did not throw std::bad_alloc");

        bool array_allocation_failed = false;
        try
        {
            (void)ShashChess::memory_allocator<int[]>(fail_to_allocate, 1);
        }
        catch (const std::bad_alloc&)
        {
            array_allocation_failed = true;
        }
        require(array_allocation_failed, "failed array allocation did not throw std::bad_alloc");
    }

    void
    test_aligned_owners()
    {
        auto value = ShashChess::make_unique_aligned<Payload>(42);
        require(value->value == 42, "aligned object was not constructed");
        require(
            reinterpret_cast<std::uintptr_t>(value.get()) % alignof(Payload) == 0,
            "aligned object has an incorrect address");

        Lifetime::constructed = 0;
        Lifetime::destroyed = 0;
        {
            auto values = ShashChess::make_unique_aligned<Lifetime[]>(3);
            require(Lifetime::constructed == 3, "aligned array elements were not constructed");
        }
        require(Lifetime::destroyed == 3, "aligned array elements were not destroyed");
    }

    void
    test_large_page_owners()
    {
        auto value = ShashChess::make_unique_large_page<Payload>(7);
        require(value->value == 7, "large-page object was not constructed");
        require(
            reinterpret_cast<std::uintptr_t>(value.get()) % 4096 == 0,
            "large-page object has an incorrect address");

        Lifetime::constructed = 0;
        Lifetime::destroyed = 0;
        {
            auto values = ShashChess::make_unique_large_page<Lifetime[]>(2);
            require(Lifetime::constructed == 2, "large-page array elements were not constructed");
        }
        require(Lifetime::destroyed == 2, "large-page array elements were not destroyed");
    }

    void
    test_align_ptr_up()
    {
        alignas(64) std::byte storage[128]{};
        auto*                 aligned = ShashChess::align_ptr_up<64>(storage + 1);
        require(
            reinterpret_cast<std::uintptr_t>(aligned) % 64 == 0,
            "align_ptr_up returned a misaligned address");
        require(
            aligned >= storage + 1 && aligned < storage + sizeof(storage),
            "align_ptr_up returned an address outside the buffer");
    }

} // namespace

int
main()
{
    test_raw_allocators();
    test_allocation_failure();
    test_aligned_owners();
    test_large_page_owners();
    test_align_ptr_up();
}
