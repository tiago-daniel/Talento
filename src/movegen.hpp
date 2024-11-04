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
    static void pawnMove(MoveList &moves, Color player, Square square, uint64 allies, uint64 enemies) {
        uint64 attackBitboard = player ? blackPawnAttacks(allies|enemies, square) : whitePawnAttacks(allies|enemies, square);

        if (attackBitboard)
            if ((square / 8 == 6 and player == WHITE) or (player == BLACK and square / 8 == 1)) {
                promotionMove(moves, square, attackBitboard, allies);
            }
            else move(moves, square, attackBitboard, allies);
    }

    static void knightMove(MoveList &moves, Square square, uint64 allies) {
        uint64 attackBitboard = knightAttacks[square];

        move(moves, square, attackBitboard, allies);
    }


    static void bishopMove(MoveList &moves, Square square, uint64 allies, uint64 enemies) {
        uint64 attackBitboard = bishopAttacks(allies|enemies, square);

        move(moves, square, attackBitboard, allies);
    }

    static void rookMove(MoveList &moves, Square square, uint64 allies, uint64 enemies) {
        uint64 attackBitboard = rookAttacks(allies|enemies, square);

        move(moves, square, attackBitboard, allies);
    }

    static void queenMove(MoveList &moves, Square square, uint64 allies, uint64 enemies) {
        uint64 attackBitboard = rookAttacks(allies|enemies, square);
        attackBitboard |= bishopAttacks(allies|enemies, square);

        move(moves, square, attackBitboard, allies);
    }

    static void kingMove(MoveList &moves, Square square, uint64 allies) {
        uint64 attackBitboard = kingAttacks[square];

        move(moves, square, attackBitboard, allies);
    }


    static void move(MoveList &moves, int sq, uint64 board, uint64 ally) {
        board &= ~ally;
        while (board) {
            moves.push(Move{static_cast<Square>(sq),static_cast<Square>(__builtin_ctzll(board)), NORMAL});
            board &= board - 1;
        }
    }
    static void promotionMove(MoveList &moves, int sq, uint64 board, uint64 ally) {
        board &= ~ally;
        while (board) {
            for (Piece p = KNIGHT; p <= QUEEN; p = Piece(p+1)) {
                std::cout << p << std::endl;
                moves.push(Move{static_cast<Square>(sq),
                    static_cast<Square>(__builtin_ctzll(board)), PROMOTION, p});
            }
            board &= board - 1;
        }
    }
};

#endif //MOVEGEN_HPP
