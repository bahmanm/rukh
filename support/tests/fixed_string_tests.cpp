#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>

#include <rukh/support/fixed_string.h>

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

    void
    test_construction_and_views()
    {
        ShashChess::FixedString<12> empty;
        require(empty.size() == 0, "default value was not empty");
        require(empty.c_str()[0] == '\0', "default value was not null-terminated");

        ShashChess::FixedString<12> value("hello");
        require(value.size() == 5, "size was incorrect");
        require(value.capacity() == 12, "capacity was incorrect");
        require(std::string_view(value.c_str()) == "hello", "C string contents were incorrect");
        require(static_cast<std::string>(value) == "hello", "string conversion was incorrect");
        require(static_cast<std::string_view>(value) == "hello", "view conversion was incorrect");
        require(value == "hello", "equality comparison failed");
        require(value != "world", "inequality comparison failed");
    }

    void
    test_mutation_and_hashing()
    {
        ShashChess::FixedString<12> value("hello");
        value += " ";
        value += ShashChess::FixedString<12>("world");
        require(value == "hello world", "concatenation produced incorrect contents");
        require(value[0] == 'h', "mutable indexing returned an incorrect character");
        require(
            std::hash<ShashChess::FixedString<12>>{}(value) ==
                std::hash<ShashChess::FixedString<12>>{}(
                    ShashChess::FixedString<12>("hello world")),
            "equal values produced different hashes");

        value.clear();
        require(value.size() == 0, "clear did not reset the size");
        require(value.c_str()[0] == '\0', "clear did not restore the terminator");
    }

} // namespace

int
main()
{
    test_construction_and_views();
    test_mutation_and_hashing();
}
