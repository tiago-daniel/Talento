//
// Created by metta on 11/3/24.
//

#ifndef MOVELIST_HPP
#define MOVELIST_HPP

#include <array>
#include "utils.hpp"

class MoveList {
private:
    std::array<Move,256> moves;
    int size = 0;
public:
    MoveList() = default;

    Move *begin() {
        return &moves[0];
    }

    Move *end() {
        return begin() + size;
    }

    std::array<Move, 256>& getMoves() {
        return moves;
    };

    [[nodiscard]] int getSize() const {
        return size;
    }

    void insert(Move move, int pos) {
        for (int i = size; i >=0; i--) {
            if (i == pos) {
                moves[i] = move;
                return;
            }
            moves[i] = moves[i-1];
        }
        size++;
    }

    void push(Move move){
        moves[size] = move;
        size++;
    };
};



#endif //MOVELIST_HPP
