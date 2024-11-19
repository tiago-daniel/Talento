//
// Created by metta on 11/5/24.
//

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "evaluation.hpp"

class Search {
public:
    static uint64 perftAux(Board& pos, int depth, int initial) {
        if (depth == 1) {
            return pos.numberLegal();
        }
        uint64 nodes = 0;
        uint64 curr;

        auto moves = pos.allMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            pos.makeMove(move);
            curr = perftAux(pos, depth - 1, initial);
            pos.unmakeMove(move);
            nodes += curr;
            if (depth == initial) std::cout << move << " - " << curr << std::endl;
        }
        return nodes;
    }
    static uint64 perft(Board& pos, int depth) {
        return perftAux(pos, depth, depth);
    }
    static int negaMax(Board& board, int depth) {
        board.checkForGameOver();
        if (board.getResult() != 2) {
            if (board.getResult() == 0) {
                return 1;
            }
            if (board.getCurrentPlayer() == WHITE) {
                if (board.getResult() == 1) {
                    return 2147483647;
                }
                return -2147483647;
            }
            if (board.getResult() == 1) {
                return -2147483647;
            }
            return 2147483647;
        }
        if (depth == 0) return Evaluation::eval(board);
        int max = - 10000;

        auto moves = board.allMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            board.makeMove(move);
            int score = -negaMax(board, depth - 1);
            board.unmakeMove(move);
            if (score > max) max = score;
        }
        return max;
    }
    static Move rootNegaMax(Board& pos, int depth) {
        Move bestMove;
        pos.checkForGameOver();
        if (pos.getResult() != 2) return Move();
        double max = - 10000;
        auto moves = pos.allMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            pos.makeMove(move);
            int score = -negaMax(pos, depth - 1);
            pos.unmakeMove(move);
            if (score > max) {
                max = score;
                bestMove = pos.allMoves().getMoves()[i];
            }
        }
        assert(bestMove != Move());
        return bestMove;
    }
};



#endif //SEARCH_HPP
