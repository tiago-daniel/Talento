//
// Created by metta on 11/5/24.
//

#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>

#include "movegen.hpp"
#include "bitboard.hpp"
#include <sstream>

#endif //BOARD_HPP

enum Result { WIN = 1, DRAW = 0, LOSS = -1 };
inline std::array values{3, 3, 5, 9, 1, 200, 0};

class Board {
private:
    std::vector<StackType> stack{};
    std::array<Bitboard, 2> colors = {};
    std::array<Bitboard, 6> boards = {};
    std::array<int, 2> materials = {};
    std::array<Piece, 64> pieces = {EMPTY};
    uint64 hash = 0;
    std::array<uint64,1024> hashHistory{};
    int hashIndex = 0;
    int drawCount = 0;
    int fullMove = 0;
    int result = 2;
    bool currentPlayer = WHITE;
public:

    explicit Board(const std::string &fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
        initAllAttackTables();
        initMagics();
        initZobrist();
        std::istringstream fenStream(fen);
        std::string pieces, player, castlingRights, passant, drawCount, fullMove;
        fenStream >> pieces >> player >> castlingRights >> passant >> drawCount >> fullMove;

        int square = 56; // Start from 'a8' (rank 8, file a)
        for (char c : pieces) {
            if (c == '/') {
                square -= 16; // Move to the next rank
            } else if (isdigit(c)) {
                square += c - '0'; // Skip empty squares
            } else {
                Piece pieceType;
                int color;
                switch (c) {
                    case 'P': pieceType = PAWN;   color = WHITE; break;
                    case 'N': pieceType = KNIGHT; color = WHITE; break;
                    case 'B': pieceType = BISHOP; color = WHITE; break;
                    case 'R': pieceType = ROOK;   color = WHITE; break;
                    case 'Q': pieceType = QUEEN;  color = WHITE; break;
                    case 'K': pieceType = KING;   color = WHITE; break;
                    case 'p': pieceType = PAWN;   color = BLACK; break;
                    case 'n': pieceType = KNIGHT; color = BLACK; break;
                    case 'b': pieceType = BISHOP; color = BLACK; break;
                    case 'r': pieceType = ROOK;   color = BLACK; break;
                    case 'q': pieceType = QUEEN;  color = BLACK; break;
                    case 'k': pieceType = KING;   color = BLACK; break;
                    default:
                        throw std::invalid_argument("Invalid FEN string: unrecognized piece type");
                }
                this->pieces[square] = pieceType;
                this->colors[color].addBit(square);
                this->boards[pieceType].addBit(square);
                this->materials[color] += values[pieceType];
                square++;
            }
        }

        this->currentPlayer = (player == "w") ? WHITE : BLACK;
        Square newPassant = a1;
        if (passant.size() == 2) {
            newPassant = static_cast<Square>(stringToSquare(passant));
            if (newPassant / 8 == 2) newPassant = Square(newPassant + 8);
            else newPassant = Square(newPassant - 8);
        }

        int canCastle = 0;
        // If the castling availability is '-', no castling rights are available
        if (castlingRights != "-") {
            for (char c : castlingRights) {
                switch (c) {
                    case 'K':
                        canCastle |= 0b1; // White can castle kingside
                    break;
                    case 'Q':
                        canCastle |= 0b10; // White can castle queenside
                    break;
                    case 'k':
                        canCastle |= 0b0100; // Black can castle kingside
                    break;
                    case 'q':
                        canCastle |= 0b1000; // Black can castle queenside
                    break;
                    default:
                        // Handle invalid characters if necessary
                            throw std::invalid_argument("Invalid castling character in FEN");
                }
            }
        }
        this->stack.emplace_back(newPassant, canCastle, EMPTY);
        if (!drawCount.empty()) this->drawCount = std::stoi(drawCount);
        if (!fullMove.empty()) this->fullMove = std::stoi(fullMove);
    }

    [[nodiscard]] MoveList pseudoLegalMoves() const {
        auto moves = MoveList();
        auto allies = this->colors[currentPlayer].getBoard();
        auto enemies = this->colors[currentPlayer xor 1].getBoard();
        for (int i = 0 ; i < 64 ; i++) {
            if (this->colors[currentPlayer].hasBit(i)) {
                switch (pieces[i]) {
                    case PAWN:
                        MoveGen::pawnMove(moves, static_cast<Color>(currentPlayer), static_cast<Square>(i) ,  allies, enemies);
                        break;
                    case KNIGHT:
                        MoveGen::knightMove(moves, static_cast<Square>(i), allies);
                        break;
                    case BISHOP:
                        MoveGen::bishopMove(moves,static_cast<Square>(i), allies,enemies);
                        break;
                    case ROOK:
                        MoveGen::rookMove(moves,static_cast<Square>(i), allies,enemies);
                        break;
                    case QUEEN:
                        MoveGen::queenMove(moves,static_cast<Square>(i), allies,enemies);
                        break;
                    case KING:
                        MoveGen::kingMove(moves,static_cast<Square>(i), allies);
                        MoveGen::castleMove(moves,static_cast<Square>(i), stack.back().getCastlingRights(), allies);
                        break;
                    default:
                        break;
                }
            }
        }
        return moves;
    }

    [[nodiscard]] Square getPassant() const {
        return stack.back().getPassant();
    }

    [[nodiscard]] int getCastlingRights() const {
        return stack.back().getCastlingRights();
    }

    [[nodiscard]] int getCaptured() const {
        return stack.back().getCaptured();
    }
};