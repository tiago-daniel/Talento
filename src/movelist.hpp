//
// Created by metta on 11/3/24.
//

#ifndef MOVELIST_HPP
#define MOVELIST_HPP

#include <array>
#include "utils.hpp"

class MoveList {
private:
    std::array<Move,256> moves{};
    int size = 0;
public:
    MoveList() = default;

    std::array<Move, 256>& getMoves() {
        return moves;
    };

    [[nodiscard]] int getSize() const {
        return size;
    }

    void push(Move move){
        moves[size] = move;
        size++;
    };
};



#endif //MOVELIST_HPP
