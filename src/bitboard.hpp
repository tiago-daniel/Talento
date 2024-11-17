//
// Created by metta on 11/3/24.
//

#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include "utils.hpp"

class Bitboard {
private:
    uint64 board = 0;
public:

    Bitboard() = default;

    explicit Bitboard(uint64 board) {
        this->board = board;
    }
    [[nodiscard]] uint64 getBoard() const {
        return board;
    }
    void addBit(int square) {
        board |= Bit(square);
    }
    [[nodiscard]] bool hasBit(int square) const {
        return board & Bit(square);
    }
    void removeBit(int square) {
        board &= ~Bit(square);
    }

    friend std::ostream& operator<<(std::ostream& os, const Bitboard& board) {
        for (int row = 7; row >= 0; --row) {
            for (int col = 0; col < 8; col++) {
                os << board.hasBit(col + row * 8) << " ";
            }
            os << std::endl;
        }
        return os;
    }

    Bitboard operator^(const Bitboard& other) const {
        return Bitboard(this->board ^ other.board);
    }
    void operator^=(const Bitboard& other) {
        this->board ^= other.board;
    }

    Bitboard operator^(const uint64& other) const {
        return Bitboard(this->board ^ other);
    }
    void operator^=(const uint64& other) {
        this->board ^= other;
    }
};



#endif //BITBOARD_HPP
