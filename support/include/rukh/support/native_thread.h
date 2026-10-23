#ifndef RUKH_SUPPORT_NATIVE_THREAD_H_INCLUDED
#define RUKH_SUPPORT_NATIVE_THREAD_H_INCLUDED

#include <cstddef>
#include <exception>
#include <functional>
#include <memory>
#include <system_error>
#include <thread>
#include <utility>

#if defined(__APPLE__) || defined(__MINGW32__) || defined(__MINGW64__) || defined(USE_PTHREADS)

#include <pthread.h>

namespace ShashChess
{

    class NativeThread
    {
            pthread_t thread{};
            bool      joinable = false;

            static constexpr std::size_t stack_size = 8 * 1024 * 1024;

            static void*
            start_routine(void* ptr) noexcept
            {
                std::unique_ptr<std::function<void()>> function(
                    static_cast<std::function<void()>*>(ptr));

                try
                {
                    (*function)();
                }
                catch (...)
                {
                    std::terminate();
                }

                return nullptr;
            }

        public:
            template <
                class Function,
                class... Args>
            explicit NativeThread(
                Function&& function,
                Args&&... args)
            {
                auto task = std::make_unique<std::function<void()>>(
                    std::bind(std::forward<Function>(function), std::forward<Args>(args)...));

                pthread_attr_t attributes;
                int            error = pthread_attr_init(&attributes);
                if (error)
                    throw std::system_error(error, std::generic_category(), "pthread_attr_init");

                struct AttributeGuard
                {
                        pthread_attr_t* attributes;

                        ~AttributeGuard()
                        {
                            pthread_attr_destroy(attributes);
                        }
                } attribute_guard{&attributes};

                error = pthread_attr_setstacksize(&attributes, stack_size);
                if (error)
                    throw std::system_error(
                        error, std::generic_category(), "pthread_attr_setstacksize");

                error = pthread_create(&thread, &attributes, start_routine, task.get());
                if (error)
                    throw std::system_error(error, std::generic_category(), "pthread_create");

                task.release();
                joinable = true;
            }

            NativeThread(const NativeThread&) = delete;
            NativeThread&
            operator=(const NativeThread&) = delete;

            ~NativeThread()
            {
                if (joinable)
                    std::terminate();
            }

            void
            join()
            {
                if (!joinable)
                    throw std::system_error(std::make_error_code(std::errc::invalid_argument));

                const int error = pthread_join(thread, nullptr);
                if (error)
                    throw std::system_error(error, std::generic_category(), "pthread_join");

                joinable = false;
            }
    };

} // namespace ShashChess

#else

namespace ShashChess
{
    using NativeThread = std::thread;
}

#endif

#endif // RUKH_SUPPORT_NATIVE_THREAD_H_INCLUDED
