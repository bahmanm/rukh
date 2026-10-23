#include <stdexcept>
#include <string>

#include <rukh/support/multi_array.h>

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
    test_one_dimensional_array()
    {
        ShashChess::MultiArray<int, 3> values{};
        require(values.size() == 3, "one-dimensional size was incorrect");
        require(!values.empty(), "non-empty array was marked empty");

        values.fill(7);
        require(values[0] == 7 && values[1] == 7 && values[2] == 7, "fill missed elements");
        values.at(1) = 9;
        require(values[1] == 9, "at did not provide mutable access");
        require(values.front() == 7 && values.back() == 7, "front or back was incorrect");

        int sum = 0;
        for (const int value : values)
            sum += value;
        require(sum == 23, "iteration returned incorrect values");

        bool threw = false;
        try
        {
            (void)values.at(values.size());
        }
        catch (const std::out_of_range&)
        {
            threw = true;
        }
        require(threw, "at did not report an out-of-range index");
    }

    void
    test_nested_array()
    {
        ShashChess::MultiArray<std::string, 2, 3> values{};
        values.fill(std::string("support"));
        require(values.size() == 2, "outer dimension was incorrect");
        require(values[0].size() == 3, "inner dimension was incorrect");
        require(values[1][2] == "support", "nested fill missed an element");

        ShashChess::MultiArray<std::string, 2, 3> other{};
        other.fill(std::string("engine"));
        values.swap(other);
        require(values[0][0] == "engine", "swap did not exchange nested values");
        require(other[1][2] == "support", "swap did not preserve nested values");
    }

} // namespace

int
main()
{
    test_one_dimensional_array();
    test_nested_array();
}
