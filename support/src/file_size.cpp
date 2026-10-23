#include <rukh/support/file_size.h>

#include <filesystem>
#include <system_error>

namespace ShashChess::file
{

    std::optional<std::uintmax_t>
    size(const std::string& path)
    {
        std::error_code error;
        const auto      result = std::filesystem::file_size(path, error);
        if (error)
            return std::nullopt;

        return result;
    }

} // namespace ShashChess::file
