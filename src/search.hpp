//
// Created by metta on 11/5/24.
//

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "evaluation.hpp"

namespace Search {
    uint64 perftAux(Board& pos, int depth, int initial) {
        if (depth == 0) {
            return 1ULL;
        }
        uint64 nodes = 0;
        uint64 curr;

        auto moves = pos.pseudoLegalMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            if (move.getType() == CASTLE) {
                if (!pos.isLegalCastle(move)) continue;
            }
            pos.makeMove(move);
            if (!pos.isLegal(move)) {
                pos.unmakeMove(move);
                continue;
            }
            curr = perftAux(pos, depth - 1, initial);
            pos.unmakeMove(move);
            nodes += curr;
            if (depth == initial) std::cout << move << " - " << curr << std::endl;
        }
        return nodes;
    }
    uint64 perft(Board& pos, int depth) {
        return perftAux(pos, depth, depth);
    }
    uint64 bulkPerft(Board& pos, int depth) {
        if (depth == 1) {
            return pos.allMoves().getSize();
        }
        uint64 nodes = 0;
        uint64 curr;

        auto moves = pos.pseudoLegalMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            if (move.getType() == CASTLE) {
                if (!pos.isLegalCastle(move)) continue;
            }
            pos.makeMove(move);
            if (!pos.isLegal(move)) {
                pos.unmakeMove(move);
                continue;
            }
            curr = bulkPerft(pos, depth - 1);
            pos.unmakeMove(move);
            nodes += curr;
        }
        return nodes;
    }

};



#endif //SEARCH_HPP
