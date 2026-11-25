#ifndef RUKH_CHESS_CORE_TYPES_H
#define RUKH_CHESS_CORE_TYPES_H

#include <cassert>
#include <cstddef>
#include <cstdint>

#if defined(_WIN64) && defined(_MSC_VER)
#include <intrin.h>
#endif
#if defined(USE_POPCNT) && defined(_MSC_VER)
#include <nmmintrin.h>
#endif
#if !defined(NO_PREFETCH) && defined(_MSC_VER)
#include <xmmintrin.h>
#endif
#if defined(USE_PEXT)
#include <immintrin.h>
#define pext(value, mask) _pext_u64(value, mask)
#else
#define pext(value, mask) 0
#endif

namespace ShashChess
{

#if defined(USE_POPCNT)
    inline constexpr bool HasPopCnt = true;
#else
    inline constexpr bool HasPopCnt = false;
#endif

#if defined(USE_PEXT)
    inline constexpr bool HasPext = true;
#else
    inline constexpr bool HasPext = false;
#endif

#if defined(IS_64BIT)
    inline constexpr bool Is64Bit = true;
#else
    inline constexpr bool Is64Bit = false;
#endif

    using Key = std::uint64_t;
    using Bitboard = std::uint64_t;

    enum Color : std::uint8_t
    {
        WHITE,
        BLACK,
        COLOR_NB = 2
    };
    inline constexpr Color Colors[2] = {WHITE, BLACK};

    enum CastlingRights : std::uint8_t
    {
        NO_CASTLING,
        WHITE_OO,
        WHITE_OOO = WHITE_OO << 1,
        BLACK_OO = WHITE_OO << 2,
        BLACK_OOO = WHITE_OO << 3,
        KING_SIDE = WHITE_OO | BLACK_OO,
        QUEEN_SIDE = WHITE_OOO | BLACK_OOO,
        WHITE_CASTLING = WHITE_OO | WHITE_OOO,
        BLACK_CASTLING = BLACK_OO | BLACK_OOO,
        ANY_CASTLING = WHITE_CASTLING | BLACK_CASTLING,
        CASTLING_RIGHT_NB = 16
    };

    enum PieceType : std::uint8_t
    {
        NO_PIECE_TYPE,
        PAWN,
        KNIGHT,
        BISHOP,
        ROOK,
        QUEEN,
        KING,
        ALL_PIECES = 0,
        PIECE_TYPE_NB = 8
    };

    enum Piece : std::uint8_t
    {
        NO_PIECE,
        W_PAWN = PAWN,
        W_KNIGHT,
        W_BISHOP,
        W_ROOK,
        W_QUEEN,
        W_KING,
        B_PAWN = PAWN + 8,
        B_KNIGHT,
        B_BISHOP,
        B_ROOK,
        B_QUEEN,
        B_KING,
        PIECE_NB = 16
    };

    enum Square : std::uint8_t
    {
        SQ_A1,
        SQ_B1,
        SQ_C1,
        SQ_D1,
        SQ_E1,
        SQ_F1,
        SQ_G1,
        SQ_H1,
        SQ_A2,
        SQ_B2,
        SQ_C2,
        SQ_D2,
        SQ_E2,
        SQ_F2,
        SQ_G2,
        SQ_H2,
        SQ_A3,
        SQ_B3,
        SQ_C3,
        SQ_D3,
        SQ_E3,
        SQ_F3,
        SQ_G3,
        SQ_H3,
        SQ_A4,
        SQ_B4,
        SQ_C4,
        SQ_D4,
        SQ_E4,
        SQ_F4,
        SQ_G4,
        SQ_H4,
        SQ_A5,
        SQ_B5,
        SQ_C5,
        SQ_D5,
        SQ_E5,
        SQ_F5,
        SQ_G5,
        SQ_H5,
        SQ_A6,
        SQ_B6,
        SQ_C6,
        SQ_D6,
        SQ_E6,
        SQ_F6,
        SQ_G6,
        SQ_H6,
        SQ_A7,
        SQ_B7,
        SQ_C7,
        SQ_D7,
        SQ_E7,
        SQ_F7,
        SQ_G7,
        SQ_H7,
        SQ_A8,
        SQ_B8,
        SQ_C8,
        SQ_D8,
        SQ_E8,
        SQ_F8,
        SQ_G8,
        SQ_H8,
        SQ_NONE,
        SQUARE_ZERO = 0,
        SQUARE_NB = 64
    };

    enum Direction : std::int8_t
    {
        NORTH = 8,
        EAST = 1,
        SOUTH = -NORTH,
        WEST = -EAST,
        NORTH_EAST = NORTH + EAST,
        SOUTH_EAST = SOUTH + EAST,
        SOUTH_WEST = SOUTH + WEST,
        NORTH_WEST = NORTH + WEST
    };

    enum File : std::uint8_t
    {
        FILE_A,
        FILE_B,
        FILE_C,
        FILE_D,
        FILE_E,
        FILE_F,
        FILE_G,
        FILE_H,
        FILE_NB
    };
    enum Rank : std::uint8_t
    {
        RANK_1,
        RANK_2,
        RANK_3,
        RANK_4,
        RANK_5,
        RANK_6,
        RANK_7,
        RANK_8,
        RANK_NB
    };

#define RUKH_CHESS_CORE_INCREMENTABLE(T)                                                           \
    constexpr T& operator++(T& value)                                                              \
    {                                                                                              \
        return value = T(int(value) + 1);                                                          \
    }                                                                                              \
    constexpr T& operator--(T& value)                                                              \
    {                                                                                              \
        return value = T(int(value) - 1);                                                          \
    }
    RUKH_CHESS_CORE_INCREMENTABLE(PieceType)
    RUKH_CHESS_CORE_INCREMENTABLE(Square)
    RUKH_CHESS_CORE_INCREMENTABLE(File)
    RUKH_CHESS_CORE_INCREMENTABLE(Rank)
#undef RUKH_CHESS_CORE_INCREMENTABLE

    constexpr Direction
    operator+(
        Direction lhs,
        Direction rhs)
    {
        return Direction(int(lhs) + int(rhs));
    }
    constexpr Direction
    operator*(
        int       lhs,
        Direction rhs)
    {
        return Direction(lhs * int(rhs));
    }
    constexpr Square
    operator+(
        Square    square,
        Direction direction)
    {
        return Square(int(square) + int(direction));
    }
    constexpr Square
    operator-(
        Square    square,
        Direction direction)
    {
        return Square(int(square) - int(direction));
    }
    constexpr Square&
    operator+=(
        Square&   square,
        Direction direction)
    {
        return square = square + direction;
    }
    constexpr Square&
    operator-=(
        Square&   square,
        Direction direction)
    {
        return square = square - direction;
    }
    constexpr Color
    operator~(Color colour)
    {
        return Color(colour ^ BLACK);
    }
    constexpr Square
    flip_rank(Square square)
    {
        return Square(square ^ SQ_A8);
    }
    constexpr Square
    flip_file(Square square)
    {
        return Square(square ^ SQ_H1);
    }
    constexpr Piece
    operator~(Piece piece)
    {
        return Piece(piece ^ 8);
    }
    constexpr CastlingRights
    operator&(
        Color          colour,
        CastlingRights rights)
    {
        return CastlingRights((colour == WHITE ? WHITE_CASTLING : BLACK_CASTLING) & rights);
    }

    constexpr Square
    make_square(
        File file,
        Rank rank)
    {
        return Square((rank << 3) + file);
    }
    constexpr Piece
    make_piece(
        Color     colour,
        PieceType type)
    {
        return Piece((colour << 3) + type);
    }
    constexpr PieceType
    type_of(Piece piece)
    {
        return PieceType(piece & 7);
    }
    constexpr Color
    color_of(Piece piece)
    {
        assert(piece != NO_PIECE);
        return Color(piece >> 3);
    }
    constexpr bool
    is_ok(Square square)
    {
        return square >= SQ_A1 && square <= SQ_H8;
    }
    constexpr File
    file_of(Square square)
    {
        return File(square & 7);
    }
    constexpr Rank
    rank_of(Square square)
    {
        return Rank(square >> 3);
    }
    constexpr Square
    relative_square(
        Color  colour,
        Square square)
    {
        return Square(square ^ (colour * 56));
    }
    constexpr Rank
    relative_rank(
        Color colour,
        Rank  rank)
    {
        return Rank(rank ^ (colour * 7));
    }
    constexpr Rank
    relative_rank(
        Color  colour,
        Square square)
    {
        return relative_rank(colour, rank_of(square));
    }
    constexpr Direction
    pawn_push(Color colour)
    {
        return colour == WHITE ? NORTH : SOUTH;
    }
    constexpr Key
    make_key(std::uint64_t seed)
    {
        return seed * 6364136223846793005ULL + 1442695040888963407ULL;
    }

    enum MoveType : std::uint16_t
    {
        NORMAL,
        PROMOTION = 1 << 14,
        EN_PASSANT = 2 << 14,
        CASTLING = 3 << 14
    };

    class Move
    {
        public:
            Move() = default;
            constexpr explicit Move(std::uint16_t value) : data(value)
            {}
            constexpr Move(
                Square from,
                Square to)
                : data((from << 6) + to)
            {}

            template <MoveType Type>
            static constexpr Move
            make(
                Square    from,
                Square    to,
                PieceType promotion = KNIGHT)
            {
                return Move(Type + ((promotion - KNIGHT) << 12) + (from << 6) + to);
            }

            constexpr Square
            from_sq() const
            {
                assert(is_ok());
                return Square((data >> 6) & 0x3F);
            }
            constexpr Square
            to_sq() const
            {
                assert(is_ok());
                return Square(data & 0x3F);
            }
            constexpr MoveType
            type_of() const
            {
                return MoveType(data & (3 << 14));
            }
            constexpr PieceType
            promotion_type() const
            {
                return PieceType(((data >> 12) & 3) + KNIGHT);
            }
            constexpr bool
            is_ok() const
            {
                return none().data != data && null().data != data;
            }
            static constexpr Move
            null()
            {
                return Move(65);
            }
            static constexpr Move
            none()
            {
                return Move(0);
            }
            constexpr bool
            operator==(const Move& other) const
            {
                return data == other.data;
            }
            constexpr bool
            operator!=(const Move& other) const
            {
                return data != other.data;
            }
            constexpr explicit
            operator bool() const
            {
                return data != 0;
            }
            constexpr std::uint16_t
            raw() const
            {
                return data;
            }

            struct MoveHash
            {
                    std::size_t
                    operator()(const Move& move) const
                    {
                        return make_key(move.data);
                    }
            };

        protected:
            std::uint16_t data;
    };

} // namespace ShashChess

#endif
