#ifndef WIN_PROBABILITY_H
#define WIN_PROBABILITY_H

#include <cstdint>
#include <string>

#include <rukh/wdl/wdl.h>

#include "../position.h"
#include "../types.h"

namespace ShashChess {
namespace WDLModel {
using WDL = Rukh::Wdl::WdlPercentages;

void    init();
WDL     get_wdl_by_material(Value value, int material);
WDL     get_wdl(Value value, const Position& pos);
uint8_t get_win_probability_by_material(Value value, int material);
uint8_t get_win_probability(Value value, const Position& pos);
uint8_t get_win_probability(Value value, int depthInPlies);
double  score_normalisation_factor(int material);
double  score_normalisation_factor(const Position& pos);
std::string wdl(Value value, const Position& pos);
}
}
#endif  // WIN_PROBABILITY_H
