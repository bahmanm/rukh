#ifndef RUKH_SUPPORT_IO_H_INCLUDED
#define RUKH_SUPPORT_IO_H_INCLUDED

#include <optional>
#include <string>

namespace ShashChess::io
{

    std::optional<std::string>
    read_file_to_string(const std::string& path);

} // namespace ShashChess::io

#endif // RUKH_SUPPORT_IO_H_INCLUDED
