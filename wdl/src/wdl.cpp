#include <rukh/wdl/wdl.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <memory>
#include <utility>

namespace Rukh::Wdl
{
    namespace
    {

        constexpr int MinScore = -4000;
        constexpr int MaxScore = 4000;
        constexpr int MinMaterial = 17;
        constexpr int MaxMaterial = 78;
        constexpr int MaterialRange = MaxMaterial - MinMaterial + 1;

        using WdlTable = std::array<WdlPercentages, (MaxScore - MinScore + 1) * MaterialRange>;

        std::pair<
            double,
            double>
        wdl_parameters(const int weightedMaterial)
        {
            const double     material = weightedMaterial / 58.0;
            constexpr double as[] = {-72.32565836, 185.93832038, -144.58862193, 416.44950446};
            constexpr double bs[] = {83.86794042, -136.06112997, 69.98820887, 47.62901433};

            const double a = (((as[0] * material + as[1]) * material + as[2]) * material) + as[3];
            const double b = (((bs[0] * material + bs[1]) * material + bs[2]) * material) + bs[3];
            return {a, b};
        }

        std::size_t
        table_index(
            const int score,
            const int weightedMaterial)
        {
            return (score - MinScore) * MaterialRange + weightedMaterial - MinMaterial;
        }

        const WdlTable&
        table()
        {
            static const auto values = [] {
                auto data = std::make_unique<WdlTable>();
                for (int score = MinScore; score <= MaxScore; ++score)
                {
                    for (int material = MinMaterial; material <= MaxMaterial; ++material)
                    {
                        const auto [a, b] = wdl_parameters(material);

                        const double win = 0.5 + 1000 / (1 + std::exp((a - double(score)) / b));
                        const double loss = 0.5 + 1000 / (1 + std::exp((a - double(-score)) / b));
                        const double draw = 1000 - win - loss;

                        (*data)[table_index(score, material)] = {
                            static_cast<std::uint8_t>(std::round(win / 10.0)),
                            static_cast<std::uint8_t>(std::round(draw / 10.0)),
                            static_cast<std::uint8_t>(std::round(loss / 10.0))};
                    }
                }
                return data;
            }();

            return *values;
        }

    } // namespace

    void
    initialise()
    {
        (void)table();
    }

    WdlPercentages
    estimate_wdl(
        const int score,
        const int weightedMaterial)
    {
        const int scoreClamp = std::clamp(score, MinScore, MaxScore);
        const int materialClamp = std::clamp(weightedMaterial, MinMaterial, MaxMaterial);
        return table()[table_index(scoreClamp, materialClamp)];
    }

    int
    expected_score_percent(
        const int score,
        const int weightedMaterial)
    {
        const WdlPercentages wdl = estimate_wdl(score, weightedMaterial);
        return int(wdl.win + wdl.draw / 2);
    }

    int
    expected_score_percent_for_depth(
        const int score,
        const int depthInPlies)
    {
        const int fullMoves = depthInPlies / 2 + 1;
        const auto [a, b] = wdl_parameters(fullMoves);

        const double win = 0.5 + 1000 / (1 + std::exp((a - double(score)) / b));
        const double loss = 0.5 + 1000 / (1 + std::exp((a - double(-score)) / b));
        const double draw = 1000 - win - loss;

        return static_cast<int>(std::round((win + draw / 2.0) / 10.0));
    }

    double
    score_normalisation_factor(const int weightedMaterial)
    {
        const double     material = std::clamp(weightedMaterial, MinMaterial, MaxMaterial) / 58.0;
        constexpr double as[] = {-37.45051876, 121.19101539, -132.78783573, 420.70576692};

        return (((as[0] * material + as[1]) * material + as[2]) * material) + as[3];
    }

} // namespace Rukh::Wdl
