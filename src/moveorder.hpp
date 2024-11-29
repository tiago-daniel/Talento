//
// Created by metta on 11/28/24.
//

#ifndef MOVEORDER_HPP
#define MOVEORDER_HPP

#include "evaluation.hpp"

namespace MoveOrder {
    static int scoreMove(Move move, const Board &game) {
        auto pieces = game.getPieces();
        Piece victim = pieces[move.getDestination()];
        Piece attacker = pieces[move.getOrigin()];
        if (move.getType() == EN_PASSANT) return 1;
        if (victim == KING or victim == EMPTY) {
            assert(victim != KING);
            return 0;
        }
        return ((victim + 1) * 10 + 6 - attacker);
    }

    static void scoreMoves(MoveList &movelist, const Board &board, uint16 move = 0) {
        auto& moves = movelist.getMoves();
        auto size = movelist.getSize();
        for (int i = 0; i < size; i++) {
            moves[i].setScore(scoreMove(moves[i],board));
            if (moves[i].getMoveCode() == move) {
                moves[i].setScore(1000);
            }
        }
    }

    static int indexHighestMove(MoveList &moves, int start) {
        int i = -1;
        int score = std::numeric_limits<int>::min();
        for (int j = start; j < moves.getSize(); j++) {
            auto move = moves.getMoves()[j];
            if (move.getScore() > score) {
                score = move.getScore();
                i = j;
            }
        }
        assert(i >= 0);
        return i;
    }

    static void sortMoves(MoveList &moves, const Board& game) {
        std::sort(moves.begin(), moves.end(), [&game](Move a, Move b)
            {
                return scoreMove(a, game) > scoreMove(b, game);
            });
    }
};



#endif //MOVEORDER_HPP
