#ifndef RUKH_SUPPORT_FILE_SIZE_H_INCLUDED
#define RUKH_SUPPORT_FILE_SIZE_H_INCLUDED

#include <cstdint>
#include <optional>
#include <string>

namespace ShashChess::file
{

    std::optional<std::uintmax_t>
    size(const std::string& path);

} // namespace ShashChess::file

#endif // RUKH_SUPPORT_FILE_SIZE_H_INCLUDED
