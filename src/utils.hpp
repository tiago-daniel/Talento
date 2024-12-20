//
// Created by metta on 11/3/24.
//

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cassert>
#include <cstdint>
#include <random>
#include <iostream>
#include <sstream>
#include <chrono>

typedef __uint128_t uint128;
typedef uint_fast64_t uint64;
typedef uint_fast32_t uint32;
typedef uint_fast16_t uint16;
typedef uint_fast8_t uint8;
typedef int64_t int64;
typedef int32_t int32;

#define infinity 32000
#define mateValue 31000
#define mateScore 30000


inline uint64 Bit(int n) {
    if (!(n < 64 and n >= 0)) return 0;
    return 1ULL << n;
}

inline uint64 millisecondsElapsed(const std::chrono::steady_clock::time_point start)
{
    return (std::chrono::steady_clock::now() - start) / std::chrono::milliseconds(1);
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
    noSquare
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

enum Color {
    WHITE,
    BLACK
};

enum MoveType {
    NORMAL,
    CASTLE,
    EN_PASSANT,
    PROMOTION
};

struct StackType {
private:
    uint32 actualVal = 0;
public:
    StackType(Square passant, int castlingRights, Piece captured, int halfMove) {
        actualVal = passant + (castlingRights << 6) + (captured << 10) + (halfMove << 13);
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
    [[nodiscard]] int getHalfMove() const {
        return int((actualVal >> 13) & 0b11111);
    }
};

struct Move {
private:
    uint16 actualMove = 0;
    int score = 0;
public:
    Move() = default;

    Move(Square origin, Square destination, MoveType type, Piece promotion = QUEEN) {
        actualMove = origin + (destination << 6) + (type << 12) + (promotion - 1 << 14);
    }

    explicit Move(uint16 actualMove) {
        this->actualMove = actualMove;
    }

    void setScore(int score) {
        this->score = score;
    }

    [[nodiscard]] int getScore() const {
        return score;
    }

    [[nodiscard]] uint16 getCode() const {
        return actualMove;
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
        return Piece((actualMove >> 14 & 0b11) + 1);}

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
            return " ";
    }
}

inline Piece stringToPiece(const char& pieceStr) {
    switch (pieceStr) {
        case 'n':
            return KNIGHT;
        case 'b':
            return BISHOP;
        case 'r':
            return ROOK;
        case 'q':
            return QUEEN;
        default:
            return PAWN;
    }
}

inline Square stringToSquare(const std::string &square) {
    if (square.length() != 2) {
        assert(false);
        return noSquare;
    }

    // Convert file character to lowercase to handle uppercase inputs
    char file_char = std::tolower(square[0]);
    char rank_char = square[1];

    // Validate file character ('a' to 'h')
    if (file_char < 'a' || file_char > 'h') {
        assert(false);
        return noSquare;
    }
    int file = file_char - 'a';


    // Validate rank character ('1' to '8')
    if (rank_char < '1' || rank_char > '8') {
        assert(false);
        return noSquare;
    }
    int rank = rank_char - '1';

    // Calculate square index (0 to 63)
    int square2 = rank * 8 + file;

    return static_cast<Square>(square2);
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

inline std::ostream& operator<<(std::ostream& os, const std::array<Piece, 64> & mailbox) {
    for (int row = 7; row >= 0; --row) {
        for (int col = 0; col < 8; col++) {
            os << pieceToString(mailbox[col + row * 8]) << " ";
        }
        os << std::endl;
    }
    return os;
}

inline std::vector<std::string> splitString(const std::string &str) {
    std::vector<std::string> strings;
    std::istringstream stream(str);
    std::string word;

    while (stream >> word) {
        strings.push_back(word);
    }

    return strings;
}

//HASHING

inline uint64 zobristTable[64][12]{};
inline uint64 blackHash = 0;
inline uint64 castleHash[4]{};
inline uint64 passantHash[8]{};

inline void initZobrist() {
    for (auto &i : zobristTable) {
        for (auto &j : i) {
            j = randomuint64();
        }
    }
    blackHash = randomuint64();
    for (auto &i : castleHash) {
        i = randomuint64();
    }
    for (auto &i : passantHash) {
        i = randomuint64();
    }
}

#endif //UTILS_HPP
