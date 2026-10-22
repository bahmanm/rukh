#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>

#include <rukh/support/io.h>

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
    test_read_file_as_bytes()
    {
        const std::filesystem::path path = "rukh-support-io-test.tmp";
        const std::string           contents("first\0second\n", 13);

        {
            std::ofstream file(path, std::ios_base::binary);
            require(file.good(), "could not create test file");
            file.write(contents.data(), static_cast<std::streamsize>(contents.size()));
        }

        const auto result = ShashChess::io::read_file_to_string(path.string());
        std::filesystem::remove(path);

        require(result.has_value(), "existing file was not read");
        require(*result == contents, "file bytes were not preserved");
    }

    void
    test_missing_file()
    {
        const std::filesystem::path path = "rukh-support-io-missing.tmp";
        std::filesystem::remove(path);
        require(
            !ShashChess::io::read_file_to_string(path.string()),
            "missing file did not return an empty optional");
    }

} // namespace

int
main()
{
    test_read_file_as_bytes();
    test_missing_file();
}
