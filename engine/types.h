#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <cassert>
#include <cstdint>
#include <type_traits>

#include <rukh/chess/core/types.h>
#include <rukh/support/value_list.h>

namespace ShashChess {

constexpr int MAX_MOVES = 256;
constexpr int MAX_PLY = 246;

enum Bound : std::uint8_t { BOUND_NONE, BOUND_UPPER, BOUND_LOWER, BOUND_EXACT = BOUND_UPPER | BOUND_LOWER };

using Value = int;
constexpr Value VALUE_ZERO = 0;
constexpr Value VALUE_DRAW = 0;
constexpr Value VALUE_KNOWN_WIN = 10000;
constexpr Value VALUE_NONE = 32002;
constexpr Value VALUE_INFINITE = 32001;
constexpr Value VALUE_MATE = 32000;
constexpr Value VALUE_MATE_IN_MAX_PLY = VALUE_MATE - MAX_PLY;
constexpr Value VALUE_MATED_IN_MAX_PLY = -VALUE_MATE_IN_MAX_PLY;
constexpr Value VALUE_TB = VALUE_MATE_IN_MAX_PLY - 1;
constexpr Value VALUE_TB_WIN_IN_MAX_PLY = VALUE_TB - MAX_PLY;
constexpr Value VALUE_TB_LOSS_IN_MAX_PLY = -VALUE_TB_WIN_IN_MAX_PLY;

constexpr bool is_valid(Value value) { return value != VALUE_NONE; }
constexpr bool is_win(Value value) { assert(is_valid(value)); return value >= VALUE_TB_WIN_IN_MAX_PLY; }
constexpr bool is_loss(Value value) { assert(is_valid(value)); return value <= VALUE_TB_LOSS_IN_MAX_PLY; }
constexpr bool is_decisive(Value value) { return is_win(value) || is_loss(value); }

constexpr Value PawnValue = 208;
constexpr Value KnightValue = 781;
constexpr Value BishopValue = 825;
constexpr Value RookValue = 1276;
constexpr Value QueenValue = 2538;
constexpr Value PieceValue[PIECE_NB] = {
    VALUE_ZERO, PawnValue, KnightValue, BishopValue, RookValue, QueenValue, VALUE_ZERO, VALUE_ZERO,
    VALUE_ZERO, PawnValue, KnightValue, BishopValue, RookValue, QueenValue, VALUE_ZERO, VALUE_ZERO};

using Depth = int;
constexpr Depth DEPTH_QS = 0;
constexpr Depth DEPTH_UNSEARCHED = -2;
constexpr Depth DEPTH_ENTRY_OFFSET = -3;

struct DirtyPiece {
    Piece pc;
    Square from, to;
    Square remove_sq, add_sq;
    Piece remove_pc, add_pc;
};

struct DirtyThreat {
    static constexpr int PcSqOffset = 0;
    static constexpr int ThreatenedSqOffset = 8;
    static constexpr int ThreatenedPcOffset = 16;
    static constexpr int PcOffset = 20;

    DirtyThreat() {}
    DirtyThreat(std::uint32_t raw) : data(raw) {}
    DirtyThreat(Piece pc, Piece threatened_pc, Square pc_sq, Square threatened_sq, bool add) :
        data((std::uint32_t(add) << 31) | (pc << PcOffset) | (threatened_pc << ThreatenedPcOffset)
             | (threatened_sq << ThreatenedSqOffset) | (pc_sq << PcSqOffset)) {}

    Piece pc() const { return static_cast<Piece>(data >> PcOffset & 0xf); }
    Piece threatened_pc() const { return static_cast<Piece>(data >> ThreatenedPcOffset & 0xf); }
    Square threatened_sq() const { return static_cast<Square>(data >> ThreatenedSqOffset & 0xff); }
    Square pc_sq() const { return static_cast<Square>(data >> PcSqOffset & 0xff); }
    bool add() const { return data >> 31; }
    std::uint32_t raw() const { return data; }

   private:
    std::uint32_t data;
};

using DirtyThreatList = ValueList<DirtyThreat, 96>;
struct DirtyThreats {
    DirtyThreatList list;
    Color us;
    Square prevKsq, ksq;
    Bitboard threatenedSqs, threateningSqs;
};

constexpr Value mate_in(int ply) { return VALUE_MATE - ply; }
constexpr Value mated_in(int ply) { return -VALUE_MATE + ply; }

template<typename T, typename... Ts>
struct is_all_same { static constexpr bool value = (std::is_same_v<T, Ts> && ...); };
template<typename... Ts>
constexpr auto is_all_same_v = is_all_same<Ts...>::value;

}  // namespace ShashChess

#endif

#include "tune.h"  // Global visibility to tuning setup
