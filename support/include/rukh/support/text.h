#ifndef RUKH_SUPPORT_TEXT_H_INCLUDED
#define RUKH_SUPPORT_TEXT_H_INCLUDED

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace ShashChess::text
{

    inline std::vector<std::string_view>
    split(
        std::string_view input,
        std::string_view delimiter)
    {
        std::vector<std::string_view> parts;
        if (input.empty())
            return parts;
        if (delimiter.empty())
        {
            parts.push_back(input);
            return parts;
        }

        std::size_t begin = 0;
        while (true)
        {
            const std::size_t end = input.find(delimiter, begin);
            if (end == std::string_view::npos)
                break;
            parts.emplace_back(input.substr(begin, end - begin));
            begin = end + delimiter.size();
        }
        parts.emplace_back(input.substr(begin));
        return parts;
    }

    inline void
    remove_whitespace(std::string& input)
    {
        input.erase(
            std::remove_if(
                input.begin(), input.end(), [](unsigned char c) { return std::isspace(c); }),
            input.end());
    }

    inline bool
    is_whitespace(std::string_view input)
    {
        return std::all_of(
            input.begin(), input.end(), [](unsigned char c) { return std::isspace(c); });
    }

    inline std::size_t
    parse_size_t(std::string_view input)
    {
        std::size_t value = 0;
        const auto  result = std::from_chars(input.data(), input.data() + input.size(), value);
        if (result.ec == std::errc::result_out_of_range)
            throw std::out_of_range("integer does not fit in size_t");
        if (input.empty() || result.ec != std::errc{} || result.ptr != input.data() + input.size())
            throw std::invalid_argument("invalid size_t value");
        return value;
    }

} // namespace ShashChess::text

#endif // RUKH_SUPPORT_TEXT_H_INCLUDED
