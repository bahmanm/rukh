#ifndef RUKH_SUPPORT_ENDIAN_H_INCLUDED
#define RUKH_SUPPORT_ENDIAN_H_INCLUDED

#include <bit>

namespace ShashChess
{

    inline constexpr bool IsLittleEndian = std::endian::native == std::endian::little;

} // namespace ShashChess

#endif // RUKH_SUPPORT_ENDIAN_H_INCLUDED
