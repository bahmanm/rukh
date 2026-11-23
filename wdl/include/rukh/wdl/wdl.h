#ifndef RUKH_WDL_WDL_H
#define RUKH_WDL_WDL_H

#include <cstdint>

namespace Rukh::Wdl
{

    struct WdlPercentages
    {
            std::uint8_t win;
            std::uint8_t draw;
            std::uint8_t loss;
    };

    void
    initialise();

    WdlPercentages
    estimate_wdl(
        int score,
        int weightedMaterial);

    int
    expected_score_percent(
        int score,
        int weightedMaterial);

    int
    expected_score_percent_for_depth(
        int score,
        int depthInPlies);

    double
    score_normalisation_factor(int weightedMaterial);

}

#endif
