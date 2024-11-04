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
};



#endif //BITBOARD_HPP
