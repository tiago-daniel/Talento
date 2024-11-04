//
// Created by metta on 11/3/24.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <random>
#include <iostream>

typedef uint_fast64_t uint64;
typedef uint_fast32_t uint32;
typedef uint_fast16_t uint16;
typedef uint_fast8_t uint8;


inline uint64 Bit(int n) {
    return 1ULL << n;
}

inline uint64_t randomuint64() {
    constexpr int rand = 0x1333317;
    static std::mt19937_64 mt(rand);
    std::uniform_int_distribution<uint64_t> dist{};
    return dist(mt);
}

enum Square {
    a1,  b1,  c1,  d1,  e1,  f1,  g1,  h1,
    a2,  b2,  c2,  d2,  e2,  f2,  g2,  h2,
    a3,  b3,  c3,  d3,  e3,  f3,  g3,  h3,
    a4,  b4,  c4,  d4,  e4,  f4,  g4,  h4,
    a5,  b5,  c5,  d5,  e5,  f5,  g5,  h5,
    a6,  b6,  c6,  d6,  e6,  f6,  g6,  h6,
    a7,  b7,  c7,  d7,  e7,  f7,  g7,  h7,
    a8,  b8,  c8,  d8,  e8,  f8,  g8,  h8,
};

enum Piece{
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,
    EMPTY
};

enum Color : bool {
    WHITE = false,
    BLACK = true
};

enum MoveType {
    NORMAL,
    CASTLE,
    EN_PASSANT,
    PROMOTION
};

struct StackType {
private:
    uint16 actualVal = 0;
public:
    StackType(Square passant, int castling_rights, Piece captured) {
        actualVal = passant * 2^0 + castling_rights * 2^6 + captured * 2^10;
    }

    [[nodiscard]] Square getPassant() const {
        return Square((actualVal >> 0) & 0b111111);
    }

    [[nodiscard]] int getCastlingRights() const {
        return int((actualVal >> 6) & 0b1111);
    }

    [[nodiscard]] Piece getCaptured() const {
        return Piece((actualVal >> 10) & 0b111);
    }
};

struct Move {
private:
    uint16 actualMove = 0;
public:
    Move() = default;

    Move(Square origin, Square destination, MoveType type, Piece promotion = KNIGHT) {
        actualMove = origin + (destination << 6) + (type << 12) + ((promotion - 1) << 14);
    }

    [[nodiscard]] Square getOrigin() const {
        return Square((actualMove >> 0) & 0b111111);
    }

    [[nodiscard]] Square getDestination() const {
        return Square((actualMove >> 6) & 0b111111);
    }

    [[nodiscard]] MoveType getType() const {
        return MoveType((actualMove >> 12) & 0b11);
    }

   [[nodiscard]] Piece getPromotion() const {
        return Piece(((actualMove >> 14) + 1) & 0b11);
    }

    bool operator==(const Move& other) const {
        return (this->actualMove == other.actualMove);
    }
};

//PRINT STUFF

inline std::string squareToString(Square square) {
    char file = static_cast<char>('a' + (square % 8));
    char rank = static_cast<char>('1' + (square / 8));

    return std::string(1, file) + rank;
}

inline std::ostream& operator<<(std::ostream& os, const Square square) {
    os << squareToString(square);
    return os;
}

inline std::string pieceToString(Piece piece) {
    switch (piece) {
        case PAWN:
            return "p";
        case KNIGHT:
            return "n";
        case BISHOP:
            return "b";
        case ROOK:
            return "r";
        case QUEEN:
            return "q";
        case KING:
            return "k";
        default:
            return "U";
    }
}

inline std::ostream& operator<<(std::ostream& os, const Piece& piece) {
    os << pieceToString(piece);
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const Move& move) {
    if (move.getType() != PROMOTION) {
        os << move.getOrigin() << move.getDestination();
    }
    else {
        os << move.getOrigin() << move.getDestination() << move.getPromotion();
    }
    return os;
}

#endif //UTILS_HPP
