#include "wdl/win_probability.h"

#include <iostream>
#include <stdexcept>

namespace
{

    using namespace ShashChess;

    void
    require(
        bool        condition,
        const char* message)
    {
        if (!condition)
            throw std::runtime_error(message);
    }

    void
    test_material_model()
    {
        WDLModel::init();

        const auto expect = [](int score, int material, int win, int draw, int loss, int expected) {
            const auto wdl = WDLModel::get_wdl_by_material(score, material);
            require(
                wdl.win == win && wdl.draw == draw && wdl.loss == loss,
                "material WDL output changed");
            require(
                WDLModel::get_win_probability_by_material(score, material) == expected,
                "material expected-score output changed");
        };

        expect(-5000, 17, 0, 0, 100, 0);
        expect(-100, 17, 0, 99, 1, 49);
        expect(0, 17, 0, 100, 0, 50);
        expect(100, 17, 1, 99, 0, 50);
        expect(0, 58, 0, 99, 0, 49);
        expect(-100, 78, 1, 94, 6, 48);
        expect(0, 78, 2, 96, 2, 50);
        expect(100, 78, 6, 94, 1, 53);
        expect(5000, 78, 100, 0, 0, 100);
    }

    void
    test_depth_model()
    {
        const auto expect = [](int score, int depth, int expected) {
            require(
                WDLModel::get_win_probability(score, depth) == expected,
                "depth-based expected-score output changed");
        };

        expect(-500, 0, 7);
        expect(0, 0, 50);
        expect(500, 0, 93);
        expect(-500, 40, 6);
        expect(500, 40, 94);
        expect(-500, 80, 6);
        expect(500, 80, 94);
    }

} // namespace

int
main()
{
    test_material_model();
    test_depth_model();
    std::cout << "16 WDL baseline cases passed\n";
}
