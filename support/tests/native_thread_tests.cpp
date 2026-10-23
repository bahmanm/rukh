#include <atomic>
#include <functional>
#include <stdexcept>
#include <system_error>

#include <rukh/support/native_thread.h>

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
    test_thread_starts_and_joins()
    {
        std::atomic<int> result{0};

        ShashChess::NativeThread thread(
            [](std::atomic<int>& value, int expected) {
                value.store(expected, std::memory_order_release);
            },
            std::ref(result), 42);

        thread.join();

        require(
            result.load(std::memory_order_acquire) == 42, "native thread did not run its callable");
    }

    void
    test_join_rejects_a_non_joinable_thread()
    {
        ShashChess::NativeThread thread([] {});
        thread.join();

        bool rejected = false;
        try
        {
            thread.join();
        }
        catch (const std::system_error&)
        {
            rejected = true;
        }

        require(rejected, "joining an already joined thread was not rejected");
    }

} // namespace

int
main()
{
    test_thread_starts_and_joins();
    test_join_rejects_a_non_joinable_thread();
}
