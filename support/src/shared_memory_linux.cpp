#include <rukh/support/shared_memory.h>

#if defined(__linux__) && !defined(__ANDROID__)

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <inttypes.h>
#include <mutex>
#include <new>
#include <pthread.h>
#include <string>
#include <vector>

#include <fcntl.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <rukh/support/hash.h>

namespace ShashChess::shm::detail
{

    namespace
    {

        struct ShmHeader
        {
                static constexpr std::uint32_t magic_value = 0xAD5F1A12;
                pthread_mutex_t                mutex;
                std::atomic<std::uint32_t>     ref_count{0};
                std::atomic<bool>              initialized{false};
                std::uint32_t                  magic = magic_value;
        };

    } // namespace

    struct SharedMemoryImpl
    {
            explicit SharedMemoryImpl(std::string shared_name) noexcept
                : name(std::move(shared_name)),
                  sentinel_base(make_sentinel_base(name))
            {}

            ~SharedMemoryImpl() noexcept
            {
                close();
            }

            bool
            open(
                std::size_t                     size,
                SharedMemoryRegion::Initializer initializer,
                const void*                     initial_value) noexcept
            {
                if (is_open())
                    return false;

                ensure_cleanup_hooks();
                data_size = size;
                total_size = size + sizeof(ShmHeader);

                bool retried_stale = false;
                while (true)
                {
                    bool created_new = false;
                    fd = shm_open(name.c_str(), O_CREAT | O_EXCL | O_RDWR, 0666);
                    if (fd == -1)
                    {
                        fd = shm_open(name.c_str(), O_RDWR, 0666);
                        if (fd == -1)
                            return false;
                    }
                    else
                        created_new = true;

                    if (!lock_file(LOCK_EX))
                    {
                        ::close(fd);
                        reset();
                        return false;
                    }

                    bool invalid_header = false;
                    bool success = created_new ? setup_new(initializer, initial_value)
                                               : setup_existing(invalid_header);
                    if (!success)
                    {
                        if (created_new || invalid_header)
                            shm_unlink(name.c_str());
                        if (mapping)
                            unmap();
                        unlock_file();
                        ::close(fd);
                        reset();
                        if (!created_new && invalid_header && !retried_stale)
                        {
                            retried_stale = true;
                            continue;
                        }
                        return false;
                    }

                    if (!lock_shared_mutex())
                    {
                        if (created_new)
                            shm_unlink(name.c_str());
                        unmap();
                        unlock_file();
                        ::close(fd);
                        reset();
                        if (!created_new && !retried_stale)
                        {
                            retried_stale = true;
                            continue;
                        }
                        return false;
                    }

                    if (!create_sentinel())
                    {
                        unlock_shared_mutex();
                        unmap();
                        if (created_new)
                            shm_unlink(name.c_str());
                        unlock_file();
                        ::close(fd);
                        reset();
                        return false;
                    }

                    header->ref_count.fetch_add(1, std::memory_order_acq_rel);
                    unlock_shared_mutex();
                    unlock_file();
                    return true;
                }
            }

            void
            close(bool skip_unmap = false) noexcept
            {
                if (fd == -1 && !mapping)
                    return;

                bool       remove_region = false;
                const bool file_locked = lock_file(LOCK_EX);
                const bool mutex_locked = file_locked && header && lock_shared_mutex();
                if (mutex_locked)
                {
                    header->ref_count.fetch_sub(1, std::memory_order_acq_rel);
                    remove_sentinel();
                    remove_region = !has_other_live_sentinels();
                    unlock_shared_mutex();
                }
                else
                {
                    remove_sentinel();
                    decrement_refcount();
                }

                if (skip_unmap)
                    mapping = nullptr;
                else
                    unmap();

                if (remove_region)
                    shm_unlink(name.c_str());
                if (file_locked)
                    unlock_file();
                if (fd != -1)
                    ::close(fd);
                if (!skip_unmap)
                    reset();
                else
                    fd = -1;
            }

            bool
            is_open() const noexcept
            {
                return fd != -1 && mapping && data;
            }

            std::uint32_t
            ref_count() const noexcept
            {
                return header ? header->ref_count.load(std::memory_order_acquire) : 0;
            }

            bool
            is_initialized() const noexcept
            {
                return header && header->initialized.load(std::memory_order_acquire);
            }

            static void
            cleanup_all(bool skip_unmap = false) noexcept
            {
                std::scoped_lock lock(registry_mutex);
                for (auto* instance : instances)
                    instance->close(skip_unmap);
                instances.clear();
            }

            static void
            register_instance(SharedMemoryImpl* instance)
            {
                std::scoped_lock lock(registry_mutex);
                instances.push_back(instance);
            }

            static void
            unregister_instance(SharedMemoryImpl* instance)
            {
                std::scoped_lock lock(registry_mutex);
                instances.erase(
                    std::remove(instances.begin(), instances.end(), instance), instances.end());
            }

            static std::mutex                     registry_mutex;
            static std::vector<SharedMemoryImpl*> instances;

            std::string name;
            int         fd = -1;
            void*       mapping = nullptr;
            void*       data = nullptr;
            ShmHeader*  header = nullptr;
            std::size_t data_size = 0;
            std::size_t total_size = 0;
            std::string sentinel_base;
            std::string sentinel_path;

        private:
            static std::string
            make_sentinel_base(const std::string& shared_name)
            {
                char buffer[32];
                std::snprintf(
                    buffer, sizeof(buffer), "sfshm_%016" PRIu64, hash_string(shared_name));
                return buffer;
            }

            static void
            handle_signal(int signal_number) noexcept
            {
                cleanup_all(true);
                struct sigaction action;
                action.sa_handler = SIG_DFL;
                sigemptyset(&action.sa_mask);
                action.sa_flags = 0;
                if (sigaction(signal_number, &action, nullptr) == -1)
                    _Exit(128 + signal_number);
                raise(signal_number);
            }

            static void
            ensure_cleanup_hooks() noexcept
            {
                static std::once_flag once;
                std::call_once(once, [] {
                    std::atexit([] { cleanup_all(true); });
                    constexpr int signals[] = {SIGHUP,  SIGINT,  SIGQUIT, SIGILL, SIGABRT, SIGFPE,
                                               SIGSEGV, SIGTERM, SIGBUS,  SIGSYS, SIGXCPU, SIGXFSZ};
                    struct sigaction action;
                    action.sa_handler = handle_signal;
                    sigemptyset(&action.sa_mask);
                    action.sa_flags = 0;
                    for (int signal_number : signals)
                        sigaction(signal_number, &action, nullptr);
                });
            }

            bool
            setup_new(
                SharedMemoryRegion::Initializer initializer,
                const void*                     initial_value) noexcept
            {
                if (ftruncate(fd, static_cast<off_t>(total_size)) == -1 ||
                    posix_fallocate(fd, 0, static_cast<off_t>(total_size)) != 0)
                    return false;
                mapping = mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                if (mapping == MAP_FAILED)
                {
                    mapping = nullptr;
                    return false;
                }
                data = mapping;
                header = reinterpret_cast<ShmHeader*>(static_cast<char*>(mapping) + data_size);
                new (header) ShmHeader{};
                initializer(data, initial_value);

                pthread_mutexattr_t attributes;
                if (pthread_mutexattr_init(&attributes) != 0)
                    return false;
                bool success =
                    pthread_mutexattr_setpshared(&attributes, PTHREAD_PROCESS_SHARED) == 0;
#if _POSIX_C_SOURCE >= 200809L
                if (success)
                    success = pthread_mutexattr_setrobust(&attributes, PTHREAD_MUTEX_ROBUST) == 0;
#endif
                if (success)
                    success = pthread_mutex_init(&header->mutex, &attributes) == 0;
                pthread_mutexattr_destroy(&attributes);
                if (!success)
                    return false;
                header->ref_count.store(0, std::memory_order_release);
                header->initialized.store(true, std::memory_order_release);
                return true;
            }

            bool
            setup_existing(bool& invalid_header) noexcept
            {
                invalid_header = false;
                struct stat status;
                if (fstat(fd, &status) == -1 ||
                    static_cast<std::size_t>(status.st_size) < total_size)
                {
                    invalid_header = true;
                    return false;
                }
                mapping = mmap(nullptr, total_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
                if (mapping == MAP_FAILED)
                {
                    mapping = nullptr;
                    return false;
                }
                data = mapping;
                header = std::launder(
                    reinterpret_cast<ShmHeader*>(static_cast<char*>(mapping) + data_size));
                if (!header->initialized.load(std::memory_order_acquire) ||
                    header->magic != ShmHeader::magic_value)
                {
                    invalid_header = true;
                    unmap();
                    return false;
                }
                return true;
            }

            bool
            lock_file(int operation) noexcept
            {
                if (fd == -1)
                    return false;
                while (flock(fd, operation) == -1)
                {
                    if (errno != EINTR)
                        return false;
                }
                return true;
            }

            void
            unlock_file() noexcept
            {
                if (fd != -1)
                    while (flock(fd, LOCK_UN) == -1 && errno == EINTR)
                    {
                    }
            }

            bool
            lock_shared_mutex() noexcept
            {
                if (!header)
                    return false;
                int result = pthread_mutex_lock(&header->mutex);
#if _POSIX_C_SOURCE >= 200809L
                if (result == EOWNERDEAD)
                    return pthread_mutex_consistent(&header->mutex) == 0;
#endif
                return result == 0;
            }

            void
            unlock_shared_mutex() noexcept
            {
                if (header)
                    pthread_mutex_unlock(&header->mutex);
            }

            std::string
            sentinel_full_path(pid_t pid) const
            {
                char buffer[1024];
                std::snprintf(
                    buffer, sizeof(buffer), "/dev/shm/%s.%ld", sentinel_base.c_str(), long(pid));
                return buffer;
            }

            void
            decrement_refcount() noexcept
            {
                if (!header)
                    return;
                std::uint32_t count = header->ref_count.load(std::memory_order_relaxed);
                while (count &&
                       !header->ref_count.compare_exchange_weak(
                           count, count - 1, std::memory_order_acq_rel, std::memory_order_relaxed))
                {
                }
            }

            bool
            create_sentinel() noexcept
            {
                sentinel_path = sentinel_full_path(getpid());
                for (int attempt = 0; attempt < 2; ++attempt)
                {
                    const int sentinel_fd = ::open(
                        sentinel_path.c_str(), O_CREAT | O_EXCL | O_WRONLY | O_CLOEXEC, 0600);
                    if (sentinel_fd != -1)
                    {
                        ::close(sentinel_fd);
                        return true;
                    }
                    if (errno != EEXIST)
                        break;
                    ::unlink(sentinel_path.c_str());
                    decrement_refcount();
                }
                sentinel_path.clear();
                return false;
            }

            void
            remove_sentinel() noexcept
            {
                if (!sentinel_path.empty())
                    ::unlink(sentinel_path.c_str());
                sentinel_path.clear();
            }

            static bool
            pid_is_alive(pid_t pid) noexcept
            {
                return pid > 0 && (kill(pid, 0) == 0 || errno == EPERM);
            }

            bool
            has_other_live_sentinels() noexcept
            {
                DIR* directory = opendir("/dev/shm");
                if (!directory)
                    return false;
                const std::string prefix = sentinel_base + ".";
                bool              found = false;
                while (dirent* entry = readdir(directory))
                {
                    const std::string entry_name = entry->d_name;
                    if (entry_name.rfind(prefix, 0) != 0)
                        continue;
                    const std::string pid_text = entry_name.substr(prefix.size());
                    char*             end = nullptr;
                    const long        value = std::strtol(pid_text.c_str(), &end, 10);
                    if (!end || *end != '\0')
                        continue;
                    const pid_t pid = static_cast<pid_t>(value);
                    if (pid_is_alive(pid))
                    {
                        found = true;
                        break;
                    }
                    ::unlink((std::string("/dev/shm/") + entry_name).c_str());
                    decrement_refcount();
                }
                closedir(directory);
                return found;
            }

            void
            unmap() noexcept
            {
                if (mapping)
                    munmap(mapping, total_size);
                mapping = nullptr;
                data = nullptr;
                header = nullptr;
            }

            void
            reset() noexcept
            {
                fd = -1;
                mapping = nullptr;
                data = nullptr;
                header = nullptr;
                sentinel_path.clear();
            }
    };

    std::mutex                     SharedMemoryImpl::registry_mutex;
    std::vector<SharedMemoryImpl*> SharedMemoryImpl::instances;

    SharedMemoryRegion::SharedMemoryRegion(const std::string& name) noexcept
        : implementation_(new(std::nothrow) SharedMemoryImpl(name))
    {}

    SharedMemoryRegion::~SharedMemoryRegion() noexcept
    {
        auto* implementation = static_cast<SharedMemoryImpl*>(implementation_);
        if (implementation)
        {
            SharedMemoryImpl::unregister_instance(implementation);
            delete implementation;
        }
    }

    SharedMemoryRegion::SharedMemoryRegion(SharedMemoryRegion&& other) noexcept
        : implementation_(
              std::exchange(
                  other.implementation_,
                  nullptr))
    {}

    SharedMemoryRegion&
    SharedMemoryRegion::operator=(SharedMemoryRegion&& other) noexcept
    {
        if (this != &other)
        {
            auto* implementation = static_cast<SharedMemoryImpl*>(implementation_);
            if (implementation)
            {
                SharedMemoryImpl::unregister_instance(implementation);
                delete implementation;
            }
            implementation_ = std::exchange(other.implementation_, nullptr);
        }
        return *this;
    }

    bool
    SharedMemoryRegion::open(
        std::size_t data_size,
        Initializer initializer,
        const void* initial_value) noexcept
    {
        auto* implementation = static_cast<SharedMemoryImpl*>(implementation_);
        if (!implementation || !implementation->open(data_size, initializer, initial_value))
            return false;
        SharedMemoryImpl::register_instance(implementation);
        return true;
    }

    void
    SharedMemoryRegion::close(bool skip_unmap) noexcept
    {
        if (auto* implementation = static_cast<SharedMemoryImpl*>(implementation_))
            implementation->close(skip_unmap);
    }

    void*
    SharedMemoryRegion::data() const noexcept
    {
        auto* implementation = static_cast<SharedMemoryImpl*>(implementation_);
        return implementation ? implementation->data : nullptr;
    }

    bool
    SharedMemoryRegion::is_open() const noexcept
    {
        auto* implementation = static_cast<SharedMemoryImpl*>(implementation_);
        return implementation && implementation->is_open();
    }

    std::uint32_t
    SharedMemoryRegion::ref_count() const noexcept
    {
        auto* implementation = static_cast<SharedMemoryImpl*>(implementation_);
        return implementation ? implementation->ref_count() : 0;
    }

    bool
    SharedMemoryRegion::is_initialized() const noexcept
    {
        auto* implementation = static_cast<SharedMemoryImpl*>(implementation_);
        return implementation && implementation->is_initialized();
    }

    const std::string&
    SharedMemoryRegion::name() const noexcept
    {
        return static_cast<SharedMemoryImpl*>(implementation_)->name;
    }

    void
    SharedMemoryRegion::cleanup_all_instances() noexcept
    {
        SharedMemoryImpl::cleanup_all();
    }

} // namespace ShashChess::shm::detail

#endif
