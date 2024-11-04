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
    static void passantMove(MoveList &moves, Color player, Square square, Square passant) {
        const auto destination = Square(player ? passant - 8 : passant + 8);
        if(square == passant + 8 or square == passant - 8) {
            moves.push(Move{square,destination, EN_PASSANT});
        }
    }
    static void castleMove(MoveList &moves, Square square, int castling_rights) {
        if (square == e1) {
            if (castling_rights & 0b1) {
                moves.push(Move{e1,h1, CASTLE});
            }
            if (castling_rights & 0b10) {
                moves.push(Move{e1,a1, CASTLE});
            }
        }
        else if (square == e8) {
            if (castling_rights & 0b100) {
                moves.push(Move{e8,h8, CASTLE});
            }
            if (castling_rights & 0b1000) {
                moves.push(Move{e8,a8, CASTLE});
            }
        }
    }
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
