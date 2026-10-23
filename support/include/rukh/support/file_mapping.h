#ifndef RUKH_SUPPORT_FILE_MAPPING_H_INCLUDED
#define RUKH_SUPPORT_FILE_MAPPING_H_INCLUDED

#include <cstddef>
#include <string>

namespace ShashChess
{

    class FileMapping
    {
        public:
            FileMapping() = default;
            ~FileMapping();

            FileMapping(const FileMapping&) = delete;
            FileMapping&
            operator=(const FileMapping&) = delete;
            FileMapping(FileMapping&& other) noexcept;
            FileMapping&
            operator=(FileMapping&& other) noexcept;

            bool
            map(const std::string& path);
            void
            unmap();

            bool
            has_data() const;
            const unsigned char*
            data() const;
            std::size_t
            data_size() const;

        private:
#ifdef _WIN32
            void* mappingHandle_ = nullptr;
#else
            std::size_t mappingSize_ = 0;
#endif
            void*       baseAddress_ = nullptr;
            std::size_t dataSize_ = 0;
    };

} // namespace ShashChess

#endif // RUKH_SUPPORT_FILE_MAPPING_H_INCLUDED
