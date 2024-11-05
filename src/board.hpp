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
    std::vector<StackType> stack;
    std::array<Bitboard, 2> colors = {};
    std::array<Bitboard, 6> boards = {};
    std::array<int, 2> materials = {};
    std::array<Piece, 64> pieces = {EMPTY};
    uint64 hash = 0;
    std::array<uint64,1024> hashHistory{};
    int hashIndex = 0;
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
                hashSquare(hash, Square(square));
                square++;
            }
        }

        this->currentPlayer = (player == "w") ? WHITE : BLACK;
        Square newPassant = a1;
        if (passant.size() == 2) {
            newPassant = (stringToSquare(passant));
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
        auto halfMove = !drawCount.empty() ? std::stoi(drawCount) : 0;
        this->stack.emplace_back(newPassant, canCastle, EMPTY, halfMove);
        if (!fullMove.empty()) this->fullMove = std::stoi(fullMove);
    }

    void hashSquare(uint64 &hash, const Square square) const {
        if (pieces[square] == EMPTY or square == noSquare) {return;}
        hash^=transpositionTable[square][pieces[square] + 6 * colors[BLACK].hasBit(square)];
    }

    [[nodiscard]] MoveList pseudoLegalMoves() {
        auto moves = MoveList();
        if (result != 2) return moves;
        auto allies = this->colors[currentPlayer].getBoard();
        auto enemies = this->colors[currentPlayer xor 1].getBoard();
        for (int i = 0 ; i < 64 ; i++) {
            if (this->colors[currentPlayer].hasBit(i)) {
                switch (pieces[i]) {
                    case PAWN:
                        MoveGen::pawnMove(moves, static_cast<Color>(currentPlayer), static_cast<Square>(i) ,  allies, enemies);
                    if (getPassant() != noSquare) MoveGen::passantMove(moves, static_cast<Color>(currentPlayer), static_cast<Square>(i) ,  getPassant());
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
        if (moves.getSize() == 0) {
            if (kingCheck(currentPlayer)) {
                result = currentPlayer == WHITE ? -1 : 1;
            }
            else {
                result = 0;
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

    [[nodiscard]] int getHalfMove() const {
        return stack.back().getHalfMove();
    }

    [[nodiscard]] int getCaptured() const {
        return stack.back().getCaptured();
    }

    uint64 squareAttackers(const Square square, bool player) {
        uint64 res  = 0;
        auto allyColor = colors[player].getBoard();
        auto enemyColor = colors[player ^ 1].getBoard();
        if (square == stack.back().getPassant()) {
            if (square % 8 > (square - 1)% 8 ) res |= Bit(square - 1) & enemyColor & boards[PAWN].getBoard();
            if (square % 8 < (square + 1)% 8 ) res |= Bit(square + 1) & enemyColor & boards[PAWN].getBoard();
        }
        res |= kingAttacks[square] & enemyColor & boards[KING].getBoard();
        res |= knightAttacks[square] & enemyColor & boards[KNIGHT].getBoard();
        res |= ( player == WHITE ? whitePawnAttacks(~Bit(square + 8),square) : blackPawnAttacks(~Bit(square - 8),square)) & enemyColor & boards[PAWN].getBoard();
        res |= rookAttacks(allyColor|enemyColor,square) & enemyColor & (boards[QUEEN].getBoard() | boards[ROOK].getBoard());
        res |= bishopAttacks(allyColor|enemyColor,square) & enemyColor & (boards[QUEEN].getBoard() | boards[BISHOP].getBoard());
        return res;
    }

    [[nodiscard]] bool isSquareAttacked(Square square, bool player) {
        return (squareAttackers(square, player) != 0);
    }

    [[nodiscard]] Square kingSquare(bool player) const {
        for (int i = 0 ; i < 64 ; i++) {
            if (this->colors[player].hasBit(i)) {
                if (this->boards[KING].hasBit(i)) {return Square(i);}
            }
        }
        assert(false);
    }

    [[nodiscard]] bool kingCheck(bool player) {
        return isSquareAttacked(kingSquare(player), player);
    }

    [[nodiscard]] bool insufficientMaterial() const {
        if (this->boards[PAWN].getBoard() != 0) {
            return false;
        }
        if (materials[WHITE] < 4 and materials[BLACK] < 4) {
            return true;
        }
        return false;
    }

    void makeMove(const Move& move) {
        fullMove = (currentPlayer == WHITE ) ? fullMove : fullMove + 1;
        auto halfMove = getHalfMove();
        auto origin = move.getOrigin();
        auto destination = move.getDestination();
        auto movingPiece = pieces[origin];
        auto oldPassant = getPassant();
        auto capturedPiece = pieces[destination];
        auto castlingRights = getCastlingRights();
        if (oldPassant != a1) hash^=passantHash[oldPassant % 8];
        auto passant = a1;
        if (move.getType() == CASTLE) {
            if (destination == origin + 2) {
                hashSquare(hash, origin);
                hashSquare(hash, Square(origin + 3));
                colors[currentPlayer].removeBit(origin);
                boards[KING].removeBit(origin);
                pieces[origin] = EMPTY;
                colors[currentPlayer].addBit(origin + 1);
                boards[ROOK].addBit(origin + 1);
                pieces[origin + 1] = ROOK;
                colors[currentPlayer].addBit(origin + 2);
                boards[KING].addBit(origin + 2);
                pieces[origin + 2] = KING;
                colors[currentPlayer].removeBit(origin + 3);
                boards[ROOK].removeBit(origin + 3);
                pieces[origin + 3] = EMPTY;
                hashSquare(hash, Square(origin + 1));
                hashSquare(hash, Square(origin + 2));
            }
            else {
                hashSquare(hash, origin);
                hashSquare(hash, Square(origin - 4));
                colors[currentPlayer].removeBit(origin);
                boards[KING].removeBit(origin);
                pieces[origin] = EMPTY;
                colors[currentPlayer].addBit(origin - 1);
                boards[ROOK].addBit(origin - 1);
                pieces[origin - 1] = ROOK;
                colors[currentPlayer].addBit(origin - 2);
                boards[KING].addBit(origin - 2);
                pieces[origin - 2] = KING;
                colors[currentPlayer].removeBit(origin - 4);
                boards[ROOK].removeBit(origin - 4);
                pieces[origin - 4] = EMPTY;
                hashSquare(hash, Square(origin - 1));
                hashSquare(hash, Square(origin - 2));
            }
            capturedPiece = EMPTY;
            halfMove++;
        }
        else if (move.getType() == EN_PASSANT) {
            materials[currentPlayer ^ 1] -= 1;
            hashSquare(hash, origin);
            hashSquare(hash, oldPassant);
            capturedPiece = PAWN;
            colors[currentPlayer].removeBit(origin);
            boards[PAWN].removeBit(origin);
            pieces[origin] = EMPTY;
            colors[currentPlayer].addBit(destination);
            boards[PAWN].addBit(destination);
            pieces[destination] = PAWN;
            colors[currentPlayer ^ 1].removeBit(oldPassant);
            boards[PAWN].removeBit(oldPassant);
            pieces[oldPassant] = EMPTY;
            halfMove = 0;
            hashSquare(hash, destination);
        }
        else if (move.getType() == PROMOTION) {
            materials[currentPlayer ^ 1] -= values[capturedPiece];
            hashSquare(hash, origin);
            hashSquare(hash, destination);
            auto promot = move.getPromotion();
            materials[currentPlayer] += values[promot]-1;
            colors[currentPlayer].removeBit(origin);
            boards[PAWN].removeBit(origin);
            pieces[origin] = EMPTY;
            colors[currentPlayer].addBit(destination);
            pieces[destination] = promot;
            if (capturedPiece != EMPTY) {
                colors[currentPlayer ^ 1].removeBit(destination);
                boards[capturedPiece].removeBit(destination);
                halfMove = 0;
            }
            else {
                halfMove++;
            }
            boards[promot].addBit(destination);
            hashSquare(hash, destination);
        }
        else {
            materials[currentPlayer ^ 1] -= values[capturedPiece];
            hashSquare(hash, origin);
            hashSquare(hash, destination);
            colors[currentPlayer].removeBit(origin);
            boards[movingPiece].removeBit(origin);
            pieces[origin] = EMPTY;
            colors[currentPlayer].addBit(destination);
            pieces[destination] = movingPiece;
            if (capturedPiece != EMPTY) {
                colors[currentPlayer ^ 1].removeBit(destination);
                boards[capturedPiece].removeBit(destination);
                halfMove = 0;
            }
            else {
                halfMove++;
            }
            boards[movingPiece].addBit(destination);
            if (movingPiece == PAWN and (origin == destination + 16 or origin == destination - 16)) {
                passant = destination;
                hash^=passantHash[destination % 8];
            }
            hashSquare(hash, destination);
        }
        if (currentPlayer == WHITE) {
            if (castlingRights & 0b1) {
                if (pieces[e1] != KING or pieces[h1] != ROOK or !colors[WHITE].hasBit(h1)) {
                    castlingRights &= ~0b1;
                    hash ^= castleHash[0];
                }
            }
            if (castlingRights & 0b10) {
                if (pieces[e1] != KING or pieces[a1] != ROOK or !colors[WHITE].hasBit(a1)) {
                    castlingRights &= ~0b10;
                    hash ^= castleHash[1];
                }
            }
        }
        else {
            if (castlingRights & 0b100) {
                if (pieces[e8] != KING or pieces[h8] != ROOK or !colors[BLACK].hasBit(h8)) {
                    castlingRights &= ~0b100;
                    hash ^= castleHash[2];
                }
            }
            if (castlingRights & 0b01000) {
                if (pieces[e8] != KING or pieces[a8] != ROOK or !colors[BLACK].hasBit(a8)) {
                    castlingRights &= ~0b1000;
                    hash ^= castleHash[3];
                }
            }
        }
        currentPlayer ^= 1;
        hash ^= blackHash;
        hashHistory[hashIndex++] = hash;
        stack.emplace_back(passant, castlingRights, capturedPiece, halfMove);
        if (halfMove >= 50 or insufficientMaterial()) {
            result = 0;
        }
        int count = 1;
        for (int i = hashIndex - 2; i >= hashIndex - 1 - halfMove; i--) {
            if (hashHistory[i] == hash) {count++;}
            if (count >= 3) result = 0;
        }
    }

    void unmakeMove(const Move& move) {
        fullMove = (currentPlayer == WHITE ) ? fullMove - 1 : fullMove;
        auto capturedPiece = stack.back().getCaptured();
        currentPlayer ^= 1;
        stack.pop_back();
        auto origin = move.getOrigin();
        auto destination = move.getDestination();
        auto movingPiece = pieces[destination];
        if (move.getType() == CASTLE) {
            if (destination == origin + 2) {
                colors[currentPlayer].addBit(origin);
                boards[KING].addBit(origin);
                pieces[origin] = KING;
                colors[currentPlayer].removeBit(origin + 1);
                boards[ROOK].removeBit(origin + 1);
                pieces[origin + 1] = EMPTY;
                colors[currentPlayer].removeBit(origin + 2);
                boards[KING].removeBit(origin + 2);
                pieces[origin + 2] = EMPTY;
                colors[currentPlayer].addBit(origin + 3);
                boards[ROOK].addBit(origin + 3);
                pieces[origin + 3] = ROOK;
            }
            else {
                colors[currentPlayer].addBit(origin);
                boards[KING].addBit(origin);
                pieces[origin] = KING;
                colors[currentPlayer].removeBit(origin - 1);
                boards[ROOK].removeBit(origin - 1);
                pieces[origin - 1] = EMPTY;
                colors[currentPlayer].removeBit(origin - 2);
                boards[KING].removeBit(origin - 2);
                pieces[origin - 2] = EMPTY;
                colors[currentPlayer].addBit(origin - 4);
                boards[ROOK].addBit(origin - 4);
                pieces[origin - 4] = ROOK;
            }
        }
        else if (move.getType() == EN_PASSANT) {
            auto passant = getPassant();
            materials[currentPlayer ^ 1] += 1;
            colors[currentPlayer].addBit(origin);
            boards[PAWN].addBit(origin);
            pieces[origin] = PAWN;
            colors[currentPlayer].removeBit(destination);
            boards[PAWN].removeBit(destination);
            pieces[destination] = EMPTY;
            colors[currentPlayer ^ 1].addBit(passant);
            boards[PAWN].addBit(passant);
            pieces[passant] = PAWN;
        }
        else if (move.getType() == PROMOTION) {
            materials[currentPlayer ^ 1] += values[capturedPiece];
            auto promot = move.getPromotion();
            materials[currentPlayer] -= values[promot]-1;
            colors[currentPlayer].addBit(origin);
            boards[PAWN].addBit(origin);
            pieces[origin] = PAWN;
            colors[currentPlayer].removeBit(destination);
            pieces[destination] = capturedPiece;
            boards[promot].removeBit(destination);
            if (capturedPiece != EMPTY) {
                colors[currentPlayer ^ 1].addBit(destination);
                boards[capturedPiece].addBit(destination);
            }
        }
        else {
            materials[currentPlayer ^ 1] += values[capturedPiece];
            colors[currentPlayer].addBit(origin);
            boards[movingPiece].addBit(origin);
            pieces[origin] = movingPiece;
            colors[currentPlayer].removeBit(destination);
            boards[movingPiece].removeBit(destination);
            pieces[destination] = capturedPiece;
            if (capturedPiece != EMPTY) {
                colors[currentPlayer ^ 1].addBit(destination);
                boards[capturedPiece].addBit(destination);
            }
        }
        hashIndex--;
        hash = hashHistory[hashIndex-1];
        hashHistory[hashIndex] = 0;
        result = 2;
    }
    bool isLegal(const Move& move) {
        if (move.getType() == CASTLE) {
            auto origin = move.getOrigin();
            auto destination = move.getDestination();
            if (origin < destination) {
                if (isSquareAttacked(origin,currentPlayer) or isSquareAttacked(Square(origin + 1),currentPlayer)
                    or isSquareAttacked(destination,currentPlayer)) {
                    return false;
                }
                return true;
            }
            if (isSquareAttacked(origin,currentPlayer) or isSquareAttacked(Square(origin - 1),currentPlayer)
                or isSquareAttacked(destination,currentPlayer) or isSquareAttacked(Square(origin - 3),currentPlayer)) {
                return false;
                }
            return true;
        }
        makeMove(move);
        if (kingCheck(currentPlayer^1)) {
            unmakeMove(move);
            return false;
        }
        unmakeMove(move);
        return true;
    }
};