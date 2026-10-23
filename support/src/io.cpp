#include <fstream>
#include <iterator>

#include <rukh/support/io.h>

namespace ShashChess::io
{

    std::optional<std::string>
    read_file_to_string(const std::string& path)
    {
        std::ifstream file(path, std::ios_base::binary);
        if (!file)
            return std::nullopt;
        return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }

} // namespace ShashChess::io
