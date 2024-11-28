//
// Created by metta on 11/5/24.
//

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "moveorder.hpp"
#include <chrono>
#include <atomic>

using namespace MoveOrder;

inline uint16 nodes = 0;
inline std::atomic_bool stop = false;

class Search {
    Move currMove = Move();
    Move bestMove = Move();
    uint8 plies = 0;
public:
    static uint64 perftAux(Board& pos, int depth, int initial) {
        if (depth == 0) {
            return 1ULL;
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

    int search(Board &board, int depth,int alpha, int beta,uint16 maxNodes = std::numeric_limits<uint16>::max(),
        std::chrono::time_point<std::chrono::steady_clock>
        startTime = std::chrono::time_point<std::chrono::steady_clock>::min(),
        uint64 milliseconds = std::numeric_limits<uint64>::max()) {

        board.checkForGameOver();
        if (board.getResult() == 0) return 0;

        if (depth == 0) return qSearch(board,alpha,beta,startTime,milliseconds);

        int max = -31000;
        auto moves = board.allMoves();
        scoreMoves(moves,board);
        for (int i = 0 ;i <  moves.getSize(); i++) {
            std::swap(moves.getMoves()[i], moves.getMoves()[indexHighestMove(moves,i)]);
            auto move = moves.getMoves()[i];
            board.makeMove(move);
            ++nodes;
            plies++;
            int score = -search(board, depth - 1,-beta, -alpha, maxNodes, startTime, milliseconds);
            plies--;
            board.unmakeMove(move);
            if (nodes > maxNodes) stop = true;
            else if (nodes % 1024 == 0) {
                if (millisecondsElapsed(startTime) >= milliseconds) stop = true;
            }
            if (stop) {
                return {};
            }
            if (score > max) {
                max = score;
                if (plies == 0) {
                    currMove = move;
                }
                alpha = std::max(max,alpha);
            }
            if (score >= beta) return max;
        }
        if (max == -31000) {
            if (!board.kingCheck(board.getCurrentPlayer())) {
                // STALEMATE
                return 0;
            }
            return max + plies;
        }

        return max;
    }

    void iterativeDeepening(Board &pos, const int64 maxDepth = std::numeric_limits<int64>::max(), const uint64 n = std::numeric_limits<uint64>::max(),
        std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now(),
        uint64 milliseconds = std::numeric_limits<uint64>::max()) {
        int i = 1;
        while (!stop and i <= maxDepth and nodes <= n) {
            nodes = 0;
            auto start = std::chrono::steady_clock::now();
            int score = search(pos, i,-std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), n, startTime, milliseconds);
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            if (stop) {break;}
            bestMove = currMove;
            std::cout << "info depth " << i << " nodes " << nodes << " nps " << nodes*1000000/duration.count()
            << " score cp " << score << std::endl;
            i++;
        }
    }

    [[nodiscard]] Move getBestMove() const {
        return bestMove;
    }

    int qSearch(Board& board, int alpha, int beta, std::chrono::time_point<std::chrono::steady_clock> startTime,
        uint64 milliseconds) {
        int standPat = Evaluation::eval(board);

        if(standPat >= beta) return standPat;
        if (standPat > alpha) alpha = standPat;

        auto moves = board.allMoves();
        scoreMoves(moves,board);
        for (int i = 0 ;i <  moves.getSize(); i++) {
            std::swap(moves.getMoves()[i], moves.getMoves()[indexHighestMove(moves,i)]);
            auto move = moves.getMoves()[i];
            if (board.getPieces()[move.getDestination()] == EMPTY) break;
            ++nodes;
            board.makeMove(move);
            int score = -qSearch(board,-beta, -alpha, startTime, milliseconds);
            board.unmakeMove(move);
            if (score >= beta) return score;
            if (score > alpha) alpha = score;
            if (nodes % 1024 == 0) {
                if (millisecondsElapsed(startTime) >= milliseconds) stop = true;
            }
            if (stop) return {};
        }
        return standPat;
    }
};



#endif //SEARCH_HPP