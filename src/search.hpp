//
// Created by metta on 11/5/24.
//

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "board.hpp"

class Search {
public:
    static uint64 perft(Board& pos, int depth) {
        return perftAux(pos, depth, depth);
    }
    static uint64 perftAux(Board& pos, int depth, int initial) {
        if (depth == 0) {
            return 1ULL;
        }
        uint64 nodes = 0;
        uint64 curr;

        auto moves = pos.pseudoLegalMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            if (!pos.isLegal(move)) continue;
            pos.makeMove(move);
            curr = perftAux(pos, depth - 1, initial);
            pos.unmakeMove(move);
            nodes += curr;
            if (depth == initial) std::cout << move << " - " << curr << std::endl;
        }
        return nodes;
    }
};



#endif //SEARCH_HPP
