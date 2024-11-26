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
inline int searchedDepth = 0;

class Search {
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
    static int negaMax(Board &board, int depth,uint16 maxNodes,
        std::chrono::time_point<std::chrono::steady_clock> startTime,  uint64 milliseconds) {
        if (stop or nodes > maxNodes) return {};
        board.checkForGameOver();
        if (board.getResult() == 0) {
            return 0;
        }
        if (depth == 0) {
            return Evaluation::eval(board);
        }
        int max = std::numeric_limits<int>::min() + 1;

        auto moves = board.allMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            board.makeMove(move);
            ++nodes;
            searchedDepth++;
            int score = -negaMax(board, depth - 1, maxNodes, startTime, milliseconds);
            searchedDepth--;
            board.unmakeMove(move);
            if (nodes > maxNodes) stop = true;
            else if (nodes % 1024 == 0) {
                if (millisecondsElapsed(startTime) >= milliseconds) stop = true;
            }
            if (score > max) max = score;
        }
        if (max == std::numeric_limits<int>::min() + 1) {
            if (!board.kingCheck(board.getCurrentPlayer())) {
                // STALEMATE
                return 0;
            }
            else return max + searchedDepth;
        }
        return  max;
    }
    static std::tuple<Move,int> rootNegaMax(Board &board, int depth,uint16 maxNodes,
        std::chrono::time_point<std::chrono::steady_clock> startTime,  uint64 milliseconds) {

        if (stop) return {};
        Move bestMove;
        board.checkForGameOver();
        if (board.getResult() == 0) return {};
        int max = std::numeric_limits<int>::min() + 1;
        auto moves = board.allMoves();
        for (int i = 0 ;i <  moves.getSize(); i++) {
            auto move = moves.getMoves()[i];
            board.makeMove(move);
            ++nodes;
            searchedDepth++;
            int score = -negaMax(board, depth - 1, maxNodes, startTime, milliseconds);
            searchedDepth--;
            board.unmakeMove(move);
            if (nodes > maxNodes) stop = true;
            else if (nodes % 1024 == 0) {
                if (millisecondsElapsed(startTime) >= milliseconds) stop = true;
            }
            if (score >= max) {
                max = score;
                bestMove = moves.getMoves()[i];
            }
        }
        if (max == std::numeric_limits<int>::min() + 1) {
            if (!board.kingCheck(board.getCurrentPlayer())) {
                // STALEMATE
                return std::make_pair(bestMove,0);
            }
            else return std::make_pair(bestMove,max + searchedDepth);
        }

        return std::make_pair(bestMove,max);
    }

    static Move iterativeDeepening(Board &pos, const int64 maxDepth, const uint64 n,
        std::chrono::time_point<std::chrono::steady_clock> startTime, uint64 milliseconds) {
        Move bestMove = Move();
        int i = 1;
        while (!stop and i <= maxDepth and nodes <= n) {
            nodes = 0;
            auto start = std::chrono::steady_clock::now();
            auto [currmove,score] = rootNegaMax(pos, i, n, startTime, milliseconds);
            auto end = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            if (stop) {break;}
            bestMove = currmove;
            std::cout << "info depth " << i << " nodes " << nodes << " nps " << nodes*1000000/duration.count()
            << " score cp " << score << std::endl;
            i++;
        }
        return bestMove;
    }

    static int scoreMove(Move move, const Board &game) {
        auto pieces = game.getPieces();
        Piece victim = pieces[move.getDestination()];
        Piece attacker = pieces[move.getOrigin()];
        if (move.getType() == EN_PASSANT) return 1;
        if (victim == KING or victim == EMPTY) {
            return 0;
        }
        return ((victim + 1) * 10 + 6 - attacker);
    }

    static void sortMoves(MoveList &moves, const Board& game) {
        std::sort(moves.begin(), moves.end(), [game](Move a, Move b)
                                          {
                                              return scoreMove(a, game) > scoreMove(b, game);
                                          });
    }
};



#endif //SEARCH_HPP
