#include <rukh/support/text.h>

#include <cstdarg>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace ShashChess::text
{

    std::string
    format_printf(
        const char* format,
        ...)
    {
        va_list args;
        va_start(args, format);

        va_list measureArgs;
        va_copy(measureArgs, args);
        const int length = std::vsnprintf(nullptr, 0, format, measureArgs);
        va_end(measureArgs);

        if (length < 0)
        {
            va_end(args);
            throw std::runtime_error("text formatting failed");
        }

        std::vector<char> buffer;
        try
        {
            buffer.resize(static_cast<std::size_t>(length) + 1);
        }
        catch (...)
        {
            va_end(args);
            throw;
        }

        va_list formatArgs;
        va_copy(formatArgs, args);
        va_end(args);

        const int written = std::vsnprintf(buffer.data(), buffer.size(), format, formatArgs);
        va_end(formatArgs);
        if (written != length)
            throw std::runtime_error("text formatting failed");

        return {buffer.data(), static_cast<std::size_t>(written)};
    }

    std::string
    format_bytes(
        std::uint64_t bytes,
        int           decimals)
    {
        constexpr std::uint64_t KB = 1024;
        constexpr std::uint64_t MB = KB * 1024;
        constexpr std::uint64_t GB = MB * 1024;
        constexpr std::uint64_t TB = GB * 1024;

        std::stringstream result;
        if (bytes < KB)
            result << bytes << " B";
        else if (bytes < MB)
            result << std::fixed << std::setprecision(decimals) << (double(bytes) / KB) << "KB";
        else if (bytes < GB)
            result << std::fixed << std::setprecision(decimals) << (double(bytes) / MB) << "MB";
        else if (bytes < TB)
            result << std::fixed << std::setprecision(decimals) << (double(bytes) / GB) << "GB";
        else
            result << std::fixed << std::setprecision(decimals) << (double(bytes) / TB) << "TB";

        return result.str();
    }

} // namespace ShashChess::text
