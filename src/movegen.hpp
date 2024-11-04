//
// Created by metta on 11/3/24.
//

#ifndef MOVEGEN_HPP
#define MOVEGEN_HPP

#include "movelist.hpp"
#include "magic.hpp"
#include "bitboard.hpp"

class MoveGen {
public:
    static void knightMove(MoveList &moves, Square square, uint64 allies) {
        uint64 attackBitboard = knightAttacks[square];

        attackBitboard &= ~allies;
        move(moves, square, attackBitboard, allies);
    }


    static void bishopMove(MoveList &moves, Square square, uint64 allies, uint64 enemies) {
        uint64 attackBitboard = bishopAttacks(allies|enemies, square);

        attackBitboard &= ~allies;
        move(moves, square, attackBitboard, allies);
    }

    static void rookMove(MoveList &moves, Square square, uint64 allies, uint64 enemies) {
        uint64 attackBitboard = rookAttacks(allies|enemies, square);

        attackBitboard &= ~allies;
        move(moves, square, attackBitboard, allies);
    }

    static void queenMove(MoveList &moves, Square square, uint64 allies, uint64 enemies) {
        uint64 attackBitboard = rookAttacks(allies|enemies, square);
        attackBitboard |= bishopAttacks(allies|enemies, square);

        attackBitboard &= ~allies;
        move(moves, square, attackBitboard, allies);
    }

    static void kingMove(MoveList &moves, Square square, uint64 allies) {
        uint64 attackBitboard = kingAttacks[square];

        attackBitboard &= ~allies;

        move(moves, square, attackBitboard, allies);
    }


    static void move(MoveList &moves, int sq, uint64 board, uint64 ally) {
        board &= ~ally;
        while (board) {
            moves.push(Move{static_cast<Square>(sq),static_cast<Square>(__builtin_ctzll(board)), NORMAL});
            board &= board - 1;
        }
    }
};

#endif //MOVEGEN_HPP
