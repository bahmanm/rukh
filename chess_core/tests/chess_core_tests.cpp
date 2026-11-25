#include <rukh/chess/core/bitboard.h>

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
    test_core_types()
    {
        require(~WHITE == BLACK, "colour toggle changed");
        require(make_piece(BLACK, KNIGHT) == B_KNIGHT, "piece encoding changed");
        require(color_of(B_KNIGHT) == BLACK, "piece colour decoding changed");
        require(type_of(B_KNIGHT) == KNIGHT, "piece type decoding changed");
        require(make_square(FILE_E, RANK_4) == SQ_E4, "square encoding changed");
        require(file_of(SQ_E4) == FILE_E && rank_of(SQ_E4) == RANK_4, "square decoding changed");

        const Move promotion = Move::make<PROMOTION>(SQ_A7, SQ_A8, QUEEN);
        require(promotion.is_ok(), "promotion move was rejected");
        require(promotion.from_sq() == SQ_A7 && promotion.to_sq() == SQ_A8, "move squares changed");
        require(
            promotion.type_of() == PROMOTION && promotion.promotion_type() == QUEEN,
            "promotion encoding changed");
        require(
            Move::null().raw() == 65 && Move::none().raw() == 0, "special move encodings changed");
    }

    void
    test_bitboard_tables_and_attacks()
    {
        Bitboards::init();

        require(distance(SQ_A1, SQ_H8) == 7, "square distance table is incorrect");
        require(line_bb(SQ_A1, SQ_A8) == FileABB, "file line table is incorrect");
        require(
            between_bb(SQ_A1, SQ_A8) == (FileABB ^ square_bb(SQ_A1)),
            "between-square table is incorrect");

        const Bitboard corners = square_bb(SQ_A1) | square_bb(SQ_H8);
        require(popcount(corners) == 2, "bitboard population count is incorrect");
        require(lsb(corners) == SQ_A1 && msb(corners) == SQ_H8, "bitboard bit scans are incorrect");
        require(PseudoAttacks[KNIGHT][SQ_D4] != 0, "knight pseudo-attacks are empty");

        const Bitboard rook_attacks = attacks_bb<ROOK>(SQ_A1, Bitboard{0});
        require(rook_attacks == 0x01010101010101feULL, "empty-board rook attacks are incorrect");

        const Bitboard blockers = square_bb(SQ_D6) | square_bb(SQ_F4);
        const Bitboard expected = square_bb(SQ_D5) | square_bb(SQ_D6) | square_bb(SQ_D3) |
                                  square_bb(SQ_D2) | square_bb(SQ_D1) | square_bb(SQ_E4) |
                                  square_bb(SQ_F4) | square_bb(SQ_C4) | square_bb(SQ_B4) |
                                  square_bb(SQ_A4);
        const Bitboard rook_attacks_with_blockers = attacks_bb<ROOK>(SQ_D4, blockers);
        require(
            rook_attacks_with_blockers == expected,
            "rook attacks do not stop on the first blocker");
    }

} // namespace

int
main()
{
    test_core_types();
    test_bitboard_tables_and_attacks();
    std::cout << "chess_core checks passed\n";
}
