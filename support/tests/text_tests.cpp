#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>

#include <rukh/support/text.h>

namespace
{

    void
    require(
        bool        condition,
        const char* message)
    {
        if (!condition)
            throw std::runtime_error(message);
    }

    template <
        typename Exception,
        typename Function>
    void
    require_throws(
        Function&&  function,
        const char* message)
    {
        try
        {
            function();
        }
        catch (const Exception&)
        {
            return;
        }
        throw std::runtime_error(message);
    }

    void
    test_split()
    {
        const auto parts = ShashChess::text::split("alpha,,beta,", ",");
        require(parts.size() == 4, "split did not preserve empty fields");
        require(parts[0] == "alpha", "split returned an incorrect first field");
        require(parts[1].empty(), "split did not preserve an interior empty field");
        require(parts[2] == "beta", "split returned an incorrect third field");
        require(parts[3].empty(), "split did not preserve a trailing empty field");
        require(ShashChess::text::split("", ",").empty(), "split of empty input was not empty");
        const auto unsplit = ShashChess::text::split("alpha", "");
        require(unsplit.size() == 1 && unsplit[0] == "alpha", "empty delimiter was not handled");
    }

    void
    test_whitespace()
    {
        std::string value = " \talpha\n beta\r";
        ShashChess::text::remove_whitespace(value);
        require(value == "alphabeta", "whitespace removal retained whitespace");
        require(ShashChess::text::is_whitespace(" \t\n"), "whitespace was not recognised");
        require(ShashChess::text::is_whitespace(""), "empty input was not treated as whitespace");
        require(!ShashChess::text::is_whitespace(" x "), "non-whitespace input was misclassified");
    }

    void
    test_parse_size_t()
    {
        require(ShashChess::text::parse_size_t("0") == 0, "zero did not parse");
        require(ShashChess::text::parse_size_t("12345") == 12345, "integer did not parse");
        require(
            ShashChess::text::parse_size_t(
                std::to_string(std::numeric_limits<std::size_t>::max())) ==
                std::numeric_limits<std::size_t>::max(),
            "maximum size_t did not parse");
        require_throws<std::invalid_argument>(
            [] { ShashChess::text::parse_size_t(""); }, "empty input was accepted");
        require_throws<std::invalid_argument>(
            [] { ShashChess::text::parse_size_t("12x"); }, "trailing characters were accepted");
        require_throws<std::invalid_argument>(
            [] { ShashChess::text::parse_size_t(" 12"); }, "leading whitespace was accepted");
        require_throws<std::invalid_argument>(
            [] { ShashChess::text::parse_size_t("-1"); }, "negative input was accepted");
        require_throws<std::out_of_range>(
            [] { ShashChess::text::parse_size_t("18446744073709551616"); },
            "out-of-range input was accepted");
    }

} // namespace

int
main()
{
    test_split();
    test_whitespace();
    test_parse_size_t();
}
