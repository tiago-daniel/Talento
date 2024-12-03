//
// Created by metta on 11/5/24.
//

#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "moveorder.hpp"
#include "tt.hpp"
#include <chrono>
#include <atomic>


inline std::atomic_bool stop = false;

class Search {
    uint16 nodes = 0;
    uint16 totalNodes = 0;
    Move currMove = Move();
    Move bestMove = Move();
    uint8 plies = 0;
public:

    TT transpositionTable = TT(32);

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
        if (nodes % 1024 == 0) {
            if (millisecondsElapsed(startTime) >= milliseconds) stop = true;
        }
        if (stop) {
            return {};
        }

        board.checkForGameOver();
        if (board.getResult() == 0) return 0;

        if (depth == 0) return qSearch(board,alpha,beta,startTime,milliseconds);

        uint16 ttMove = 0;
        uint64 hash = board.getHash();
        TTEntry &ttEntry = transpositionTable.entry(hash);
        if (ttEntry.zobrist == hash) ttMove = ttEntry.actualMove;

        int max = -31000;
        MoveList moves = board.allMoves();
        bool alphaChanged = false;
        MoveOrder::scoreMoves(moves,board,Move(ttMove));
        for (int i = 0 ;i <  moves.getSize(); i++) {
            std::swap(moves.getMoves()[i], moves.getMoves()[MoveOrder::indexHighestMove(moves,i)]);
            board.makeMove(moves.getMoves()[i]);
            ++nodes;
            plies++;
            int score = -search(board, depth - 1,-beta, -alpha, maxNodes, startTime, milliseconds);
            plies--;
            board.unmakeMove(moves.getMoves()[i]);
            if (nodes > maxNodes) stop = true;
            if (stop) {
                return {};
            }
            if (score > max) {
                max = score;
                if (plies == 0) {
                    currMove = moves.getMoves()[i];
                }
                if (max > alpha) {
                    alpha = max;
                    alphaChanged = true;
                }
            }
            if (score >= beta) {
                if (alphaChanged) {
                    transpositionTable.update(hash, currMove.getCode());
                }
                return max;
            }
        }
        if (max == -31000) {
            if (!board.kingCheck(board.getCurrentPlayer())) {
                // STALEMATE
                return 0;
            }
            return max + plies;
        }
        if (alphaChanged) {
            transpositionTable.update(hash, currMove.getCode());
        }
        return max;
    }

    void iterativeDeepening(Board &pos, const int64 maxDepth = std::numeric_limits<int64>::max(), const uint64 n = std::numeric_limits<uint64>::max(),
        std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now(),
        uint64 milliseconds = std::numeric_limits<uint64>::max(),
        bool printResult = false) {
        int i = 1;
        while (!stop and i <= maxDepth and nodes <= n) {
            nodes = 0;
            auto start = std::chrono::steady_clock::now();
            int score = search(pos, i,-std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), n, startTime, milliseconds);
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            totalNodes += nodes;
            if (stop) {break;}
            bestMove = currMove;
            if (printResult) {
                std::cout   << "info depth " << i << " nodes " << nodes << " nps " << nodes*1000000/duration.count()
                            << " score cp " << score << std::endl;
            }
            if (millisecondsElapsed(startTime) > milliseconds / 4) break;
            i++;
        }
    }

    [[nodiscard]] Move getBestMove() const {
        if (bestMove != Move()) return bestMove;
        return currMove;
    }

    [[nodiscard]] uint16 getNodes() const {
        return nodes;
    }

    [[nodiscard]] uint16 getTotalNodes() const {
        return totalNodes;
    }

    int qSearch(Board& board, int alpha, int beta, std::chrono::time_point<std::chrono::steady_clock> startTime,
        uint64 milliseconds) {
        int standPat = Evaluation::eval(board);

        if(standPat >= beta) return standPat;
        if (standPat > alpha) alpha = standPat;

        auto moves = board.allMoves();
        MoveOrder::scoreMoves(moves,board);
        for (int i = 0 ;i <  moves.getSize(); i++) {
            std::swap(moves.getMoves()[i], moves.getMoves()[MoveOrder::indexHighestMove(moves,i)]);
            if (board.getPieces()[moves.getMoves()[i].getDestination()] == EMPTY) break;
            ++nodes;
            board.makeMove(moves.getMoves()[i]);
            int score = -qSearch(board,-beta, -alpha, startTime, milliseconds);
            board.unmakeMove(moves.getMoves()[i]);
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