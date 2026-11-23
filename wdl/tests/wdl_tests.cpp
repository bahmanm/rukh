#include <rukh/wdl/wdl.h>

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace
{

    using Rukh::Wdl::WdlPercentages;

    void
    require(
        bool        condition,
        const char* message)
    {
        if (!condition)
            throw std::runtime_error(message);
    }

    void
    expect_wdl(
        int score,
        int material,
        int win,
        int draw,
        int loss,
        int expected)
    {
        const WdlPercentages wdl = Rukh::Wdl::estimate_wdl(score, material);
        require(
            wdl.win == win && wdl.draw == draw && wdl.loss == loss, "material WDL output changed");
        require(
            Rukh::Wdl::expected_score_percent(score, material) == expected,
            "material expected-score output changed");
    }

    void
    test_material_model()
    {
        Rukh::Wdl::initialise();

        expect_wdl(-5000, 17, 0, 0, 100, 0);
        expect_wdl(-100, 17, 0, 99, 1, 49);
        expect_wdl(0, 17, 0, 100, 0, 50);
        expect_wdl(100, 17, 1, 99, 0, 50);
        expect_wdl(0, 58, 0, 99, 0, 49);
        expect_wdl(-100, 78, 1, 94, 6, 48);
        expect_wdl(0, 78, 2, 96, 2, 50);
        expect_wdl(100, 78, 6, 94, 1, 53);
        expect_wdl(5000, 78, 100, 0, 0, 100);

        require(
            Rukh::Wdl::estimate_wdl(0, 0).win == Rukh::Wdl::estimate_wdl(0, 17).win &&
                Rukh::Wdl::estimate_wdl(0, 0).draw == Rukh::Wdl::estimate_wdl(0, 17).draw &&
                Rukh::Wdl::estimate_wdl(0, 0).loss == Rukh::Wdl::estimate_wdl(0, 17).loss,
            "material below the supported range was not clamped");
        require(
            Rukh::Wdl::estimate_wdl(0, 100).win == Rukh::Wdl::estimate_wdl(0, 78).win &&
                Rukh::Wdl::estimate_wdl(0, 100).draw == Rukh::Wdl::estimate_wdl(0, 78).draw &&
                Rukh::Wdl::estimate_wdl(0, 100).loss == Rukh::Wdl::estimate_wdl(0, 78).loss,
            "material above the supported range was not clamped");
    }

    void
    test_depth_model()
    {
        const auto expect = [](int score, int depth, int expected) {
            require(
                Rukh::Wdl::expected_score_percent_for_depth(score, depth) == expected,
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

    void
    test_score_normalisation_factor()
    {
        const auto expect = [](int material, double expected) {
            require(
                std::abs(Rukh::Wdl::score_normalisation_factor(material) - expected) < 1e-9,
                "score normalisation factor changed");
        };

        expect(17, 391.253650451367);
        expect(58, 371.65842782);
        expect(78, 370.223129313079);
        expect(0, 391.253650451367);
        expect(100, 370.223129313079);
    }

} // namespace

int
main()
{
    test_material_model();
    test_depth_model();
    test_score_normalisation_factor();
    std::cout << "23 WDL regression cases passed\n";
}
