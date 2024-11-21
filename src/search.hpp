//
// Created by metta on 11/5/24.
//

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "evaluation.hpp"
#include <chrono>
#include <atomic>

inline uint16 nodes = 0;
inline std::atomic_bool stop = false;
class Search {
public:
    static uint64 perftAux(Board& pos, int depth, int initial) {
        if (depth == 0) {
            return 1ULL;
        }
        uint64 nodes = 0;
        uint64 curr;

        auto moves = pos.pseudoLegalMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
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
    static uint64 perft(Board& pos, int depth) {
        return perftAux(pos, depth, depth);
    }
    static int negaMax(Board& board, int depth) {
        if (stop) return {};
        board.checkForGameOver();
        if (board.getResult() != 2) {
            return 1;
        }
        if (depth == 0) {
            return Evaluation::eval(board);
        }
        int max = - 10000;

        auto moves = board.allMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            board.makeMove(move);
            ++nodes;
            int score = -negaMax(board, depth - 1);
            board.unmakeMove(move);
            if (score > max) max = score;
        }
        return max;
    }
    static Move rootNegaMax(Board& pos, int depth) {
        if (stop) return {};
        Move bestMove;
        pos.checkForGameOver();
        if (pos.getResult() != 2) return {};
        double max = - 10000;
        auto moves = pos.allMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            pos.makeMove(move);
            ++nodes;
            int score = -negaMax(pos, depth - 1);
            pos.unmakeMove(move);
            if (score >= max) {
                max = score;
                bestMove = pos.allMoves().getMoves()[i];
            }
        }

        return bestMove;
    }

    static Move iterativeDeepening(Board& pos, const int64 maxDepth, const uint64 maxNodes) {
        Move bestMove = Move();
        int i = 1;
        while (!stop and i <= maxDepth and nodes <= maxNodes) {
            nodes = 0;
            auto start = std::chrono::high_resolution_clock::now();
            Move currmove = rootNegaMax(pos,i);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            if (stop) {break;}
            bestMove = currmove;
            std::cout << "info depth " << i << " nodes " << nodes << " nps " << nodes*1000000/duration.count() << std::endl;
            i++;
        }
        return bestMove;
    }
};



#endif //SEARCH_HPP
