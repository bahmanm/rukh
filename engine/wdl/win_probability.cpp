#include "win_probability.h"

#include <sstream>

namespace ShashChess {
namespace WDLModel {
namespace {

int weighted_material(const Position& pos) {
    return pos.count<PAWN>() + 3 * pos.count<KNIGHT>() + 3 * pos.count<BISHOP>()
         + 5 * pos.count<ROOK>() + 9 * pos.count<QUEEN>();
}

}  // namespace

void init() {
    Rukh::Wdl::initialise();
}

WDL get_wdl_by_material(const Value value, const int material) {
    return Rukh::Wdl::estimate_wdl(value, material);
}

WDL get_wdl(const Value value, const Position& pos) {
    return get_wdl_by_material(value, weighted_material(pos));
}

uint8_t get_win_probability_by_material(const Value value, const int material) {
    return static_cast<uint8_t>(Rukh::Wdl::expected_score_percent(value, material));
}

uint8_t get_win_probability(const Value value, const Position& pos) {
    return get_win_probability_by_material(value, weighted_material(pos));
}

uint8_t get_win_probability(const Value value, const int depthInPlies) {
    return static_cast<uint8_t>(Rukh::Wdl::expected_score_percent_for_depth(value, depthInPlies));
}

double score_normalisation_factor(const int material) {
    return Rukh::Wdl::score_normalisation_factor(material);
}

double score_normalisation_factor(const Position& pos) {
    return score_normalisation_factor(weighted_material(pos));
}

std::string wdl(const Value value, const Position& pos) {
    const WDL result = get_wdl(value, pos);
    std::stringstream ss;
    ss << int(result.win * 10) << " " << int(result.draw * 10) << " " << int(result.loss * 10);
    return ss.str();
}

}  // namespace WDLModel
}  // namespace ShashChess
