#include <rukh/support/file_mapping.h>

#include <cassert>
#include <utility>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#if __has_include(<features.h>)
#include <features.h>
#endif
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace ShashChess
{

    FileMapping::~FileMapping()
    {
        unmap();
    }

    FileMapping::FileMapping(FileMapping&& other) noexcept
    {
        *this = std::move(other);
    }

    FileMapping&
    FileMapping::operator=(FileMapping&& other) noexcept
    {
        if (this == &other)
            return *this;

        unmap();
#ifdef _WIN32
        mappingHandle_ = std::exchange(other.mappingHandle_, nullptr);
#else
        mappingSize_ = std::exchange(other.mappingSize_, 0);
#endif
        baseAddress_ = std::exchange(other.baseAddress_, nullptr);
        dataSize_ = std::exchange(other.dataSize_, 0);
        return *this;
    }

    bool
    FileMapping::map(const std::string& path)
    {
        unmap();

#ifdef _WIN32
        HANDLE file = CreateFile(
            path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
            FILE_FLAG_RANDOM_ACCESS, nullptr);
        if (file == INVALID_HANDLE_VALUE)
            return false;

        LARGE_INTEGER fileSize{};
        if (!GetFileSizeEx(file, &fileSize) || fileSize.QuadPart <= 0)
        {
            CloseHandle(file);
            return false;
        }

        HANDLE mapping = CreateFileMapping(
            file, nullptr, PAGE_READONLY, fileSize.HighPart, fileSize.LowPart, nullptr);
        CloseHandle(file);
        if (!mapping)
            return false;

        void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, 0);
        if (!view)
        {
            CloseHandle(mapping);
            return false;
        }

        mappingHandle_ = mapping;
        baseAddress_ = view;
        dataSize_ = static_cast<std::size_t>(fileSize.QuadPart);
#else
        const int file = ::open(path.c_str(), O_RDONLY);
        if (file == -1)
            return false;

        struct stat fileInfo{};
        if (fstat(file, &fileInfo) != 0 || fileInfo.st_size <= 0)
        {
            ::close(file);
            return false;
        }

        void* view = mmap(
            nullptr, static_cast<std::size_t>(fileInfo.st_size), PROT_READ, MAP_SHARED, file, 0);
        ::close(file);
        if (view == MAP_FAILED)
            return false;

#if defined(MADV_RANDOM)
        madvise(view, static_cast<std::size_t>(fileInfo.st_size), MADV_RANDOM);
#endif
        mappingSize_ = static_cast<std::size_t>(fileInfo.st_size);
        baseAddress_ = view;
        dataSize_ = mappingSize_;
#endif
        return true;
    }

    void
    FileMapping::unmap()
    {
#ifdef _WIN32
        assert(
            (mappingHandle_ == nullptr) == (baseAddress_ == nullptr) &&
            (baseAddress_ == nullptr) == (dataSize_ == 0));

        if (baseAddress_)
            UnmapViewOfFile(baseAddress_);
        if (mappingHandle_)
            CloseHandle(static_cast<HANDLE>(mappingHandle_));
        mappingHandle_ = nullptr;
#else
        assert(
            (mappingSize_ == 0) == (baseAddress_ == nullptr) &&
            (baseAddress_ == nullptr) == (dataSize_ == 0));

        if (baseAddress_ && mappingSize_)
            munmap(baseAddress_, mappingSize_);
        mappingSize_ = 0;
#endif
        baseAddress_ = nullptr;
        dataSize_ = 0;
    }

    bool
    FileMapping::has_data() const
    {
        return baseAddress_ != nullptr && dataSize_ != 0;
    }

    const unsigned char*
    FileMapping::data() const
    {
        assert(has_data());
        return static_cast<const unsigned char*>(baseAddress_);
    }

    std::size_t
    FileMapping::data_size() const
    {
        assert(has_data());
        return dataSize_;
    }

} // namespace ShashChess
