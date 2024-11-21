//
// Created by metta on 11/5/24.
//

#ifndef BOARD_HPP
#define BOARD_HPP
#include <array>

#include "movegen.hpp"
#include "bitboard.hpp"
#include <sstream>

#include "evaluation.hpp"

#endif //BOARD_HPP

enum Result { WIN = 1, DRAW = 0, LOSS = -1 };
inline std::array values{3, 3, 5, 9, 1 ,200 ,0 };

class Board {
private:
    std::vector<StackType> stack;
    std::array<Bitboard, 2> colors = {};
    std::array<Bitboard, 6> boards = {};
    std::array<int, 2> materials = {};
    std::array<Piece, 64> pieces = {
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,
        EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY
    };
    uint64 hash = 0;
    std::array<uint64,1024> hashHistory{};
    uint32 hashIndex = 0;
    int fullMove = 0;
    int result = 2;
    bool currentPlayer = WHITE;
public:

    explicit Board(const std::string &fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
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

    [[nodiscard]] std::array<Bitboard, 6>& getBoards() {
        return this->boards;
    }

    void hashSquare(uint64 &hash, const Square square) const {
        if (pieces[square] == EMPTY or square == noSquare) {return;}
        hash^=zobristTable[square][pieces[square] + 6 * colors[BLACK].hasBit(square)];
    }

    [[nodiscard]] bool getCurrentPlayer() const {
        return this->currentPlayer;
    }

    [[nodiscard]] uint64 getHash() const {
        return this->hash;
    }


    [[nodiscard]] int doubledPawns(bool player) const {
        uint64 board = colors[player].getBoard() & boards[PAWN].getBoard();
        int res = 0;
        for (int file = 0; file < 8; file++) {
            int count = 0;
            for (int rank = 0; rank < 8; rank++) {
                if (board & Bit(rank * 8 + file)) count++;
            }
            if (count > 1) res += count;
        }
        return res;
    }

    void endGame(const int result) {
        this->result = result;
    }

    [[nodiscard]] int getMaterial(bool player) const {
        return materials[player];
    }

    bool checkIntegrity() {
        for (int i = 0; i < 64; i++) {
            if (pieces[i] != EMPTY) {
                for (int j = 0; j < 6; j++) {
                    if (j == pieces[i] and !boards[j].hasBit(i)) {
                        return false;
                    }
                    if (boards[j].hasBit(i)) {
                        if  (!(colors[WHITE].hasBit(i) ^ colors[BLACK].hasBit(i))) {
                           return false;
                        }
                        if (j != pieces[i]) {
                            std::cout << Square(i) << " Board of piece " << j << " has this piece but on mailbox it's " << pieces[i]
                            << std::endl;
                            return false;
                        }
                    }
                }
            }
            else {
                if (colors[WHITE].hasBit(i) || colors[BLACK].hasBit(i)) {
                   return false;
                }
                for (int j = 0; j < 6; j++) {
                    if (boards[j].hasBit(i)) {
                        return false;
                    }
                }
            }
        }
        return true;
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

    [[nodiscard]] int numberLegal() {
        int moves = 0;
        auto oldMoves = pseudoLegalMoves();
        for (int i = 0; i < oldMoves.getSize();i++) {
            auto move = oldMoves.getMoves()[i];
            if (move.getType() == CASTLE) {
                if (isLegalCastle(move)) moves++;
            }
            else {
                makeMove(move);
                if (!isLegal(move)) {
                    unmakeMove(move);
                    continue;
                }
                unmakeMove(move);
                moves++;
            }
        }
        return moves;
    }

    [[nodiscard]] MoveList allMoves() {
        auto moves = MoveList();
        auto oldMoves = pseudoLegalMoves();
        for (int i = 0; i < oldMoves.getSize();i++) {
            auto move = oldMoves.getMoves()[i];
            if (move.getType() == CASTLE) {
                if (isLegalCastle(move)) moves.push(move);
            }
            else {
                makeMove(move);
                if (!isLegal(move)) {
                    unmakeMove(move);
                    continue;
                }
                unmakeMove(move);
                moves.push(move);
            }
        }
        if (moves.getSize() == 0) {
            if (kingCheck(currentPlayer)) {
                endGame(currentPlayer * 2 - 1 );
            }
            else endGame(0);
        }
        return moves;
    }

    [[nodiscard]] Square getPassant() const {
        return stack.back().getPassant();
    }

    [[nodiscard]] int getResult() const {
        return this->result;
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
        res |= ( player == WHITE ? whitePawnAttacks(~Bit(square - 8),square) : blackPawnAttacks(~Bit(square + 8),square)) & enemyColor & boards[PAWN].getBoard();
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

    Move stringToMove(const std::string &move) {
        const Square from = stringToSquare(move.substr(0, 2));
        const Square to = stringToSquare(move.substr(2, 4));
        const Piece movingPiece = pieces[from];
        const Piece capturedPiece = pieces[to];

        if (move.size() == 5) {
            return Move{from,to,PROMOTION,stringToPiece(move.back())};
        }
        if (capturedPiece == EMPTY and movingPiece == PAWN and to != from + 8) {
            return Move{from,to,EN_PASSANT};
        }
        if (movingPiece == KING and abs(to-from) == 2) {
            return Move{from,to,CASTLE};
        }
        return Move{from,to,NORMAL};
    }

    /**
     * @brief Returns true if there is a draw by insufficient material.
     *
     *  Thanks to Matt (nocturn9x) for providing the pseudo code for this function.
     *
     * @return  true if there's a draw by insufficient material, false otherwise
     */

    [[nodiscard]] bool insufficientMaterial() const {
        int occupancy = colors[WHITE].countBits() + colors[BLACK].countBits() > 4;
        if (occupancy > 4) return false;

        if (occupancy == 2) return true;

        if (boards[PAWN].countBits() != 0) return false;
        if (boards[ROOK].countBits() != 0) return false;
        if (boards[QUEEN].countBits() != 0) return false;

        int knightCount = boards[KNIGHT].countBits();

        if (knightCount > 1) return false;

        int bishopCount = boards[BISHOP].countBits();

        if  (bishopCount + knightCount > 1) return false;

        return true;
    }

    void updateBitboard(Square sq, Piece piece, bool color) {
        uint64 mask = Bit(sq);
        boards[piece] ^= mask;
        colors[color] ^= mask;
    }

    void makeCastleMove(Square from, Square to) {
        hashSquare(hash,from);
        if (from == e1) {
            if (to == g1) {
                hashSquare(hash,h1);
                colors[WHITE].removeBit(h1);
                colors[WHITE].removeBit(e1);
                colors[WHITE].addBit(f1);
                colors[WHITE].addBit(g1);
                boards[ROOK].removeBit(h1);
                boards[ROOK].addBit(f1);
                boards[KING].removeBit(e1);
                boards[KING].addBit(g1);
                pieces[e1] = EMPTY;
                pieces[h1] = EMPTY;
                pieces[f1] = ROOK;
                pieces[g1] = KING;
                hashSquare(hash,to);
                hashSquare(hash,f1);
                return;
            }
            hashSquare(hash,a1);
            colors[WHITE].removeBit(a1);
            colors[WHITE].removeBit(e1);
            colors[WHITE].addBit(c1);
            colors[WHITE].addBit(d1);
            boards[ROOK].removeBit(a1);
            boards[ROOK].addBit(d1);
            boards[KING].removeBit(e1);
            boards[KING].addBit(c1);
            pieces[e1] = EMPTY;
            pieces[a1] = EMPTY;
            pieces[d1] = ROOK;
            pieces[c1] = KING;
            hashSquare(hash,to);
            hashSquare(hash,d1);
        }
        else {
            if (to == g8) {
                hashSquare(hash,h8);
                colors[BLACK].removeBit(h8);
                colors[BLACK].removeBit(e8);
                colors[BLACK].addBit(f8);
                colors[BLACK].addBit(g8);
                boards[ROOK].removeBit(h8);
                boards[ROOK].addBit(f8);
                boards[KING].removeBit(e8);
                boards[KING].addBit(g8);
                pieces[e8] = EMPTY;
                pieces[h8] = EMPTY;
                pieces[f8] = ROOK;
                pieces[g8] = KING;
                hashSquare(hash,to);
                hashSquare(hash,f8);
                return;
            }
            hashSquare(hash,a8);
            colors[BLACK].removeBit(a8);
            colors[BLACK].removeBit(e8);
            colors[BLACK].addBit(c8);
            colors[BLACK].addBit(d8);
            boards[ROOK].removeBit(a8);
            boards[ROOK].addBit(d8);
            boards[KING].removeBit(e8);
            boards[KING].addBit(c8);
            pieces[e8] = EMPTY;
            pieces[a8] = EMPTY;
            pieces[d8] = ROOK;
            pieces[c8] = KING;
            hashSquare(hash,to);
            hashSquare(hash,d8);
        }
    }

    void unmakeCastleMove(Square from, Square to) {
        if (from == e1) {
            if (to == g1) {
                colors[WHITE].addBit(h1);
                colors[WHITE].addBit(e1);
                colors[WHITE].removeBit(f1);
                colors[WHITE].removeBit(g1);
                boards[ROOK].addBit(h1);
                boards[ROOK].removeBit(f1);
                boards[KING].addBit(e1);
                boards[KING].removeBit(g1);
                pieces[e1] = KING;
                pieces[h1] = ROOK;
                pieces[f1] = EMPTY;
                pieces[g1] = EMPTY;
                return;
            }
            else if (to == c1) {
                colors[WHITE].addBit(a1);
                colors[WHITE].addBit(e1);
                colors[WHITE].removeBit(c1);
                colors[WHITE].removeBit(d1);
                boards[ROOK].addBit(a1);
                boards[ROOK].removeBit(d1);
                boards[KING].addBit(e1);
                boards[KING].removeBit(c1);
                pieces[e1] = KING;
                pieces[a1] = ROOK;
                pieces[d1] = EMPTY;
                pieces[c1] = EMPTY;
                return;
            }
            assert(false);
        }
        else {
            if (to == g8) {
                colors[BLACK].addBit(h8);
                colors[BLACK].addBit(e8);
                colors[BLACK].removeBit(f8);
                colors[BLACK].removeBit(g8);
                boards[ROOK].addBit(h8);
                boards[ROOK].removeBit(f8);
                boards[KING].addBit(e8);
                boards[KING].removeBit(g8);
                pieces[e8] = KING;
                pieces[h8] = ROOK;
                pieces[f8] = EMPTY;
                pieces[g8] = EMPTY;
                return;
            }
            else if (to == c8) {
                colors[BLACK].addBit(a8);
                colors[BLACK].addBit(e8);
                colors[BLACK].removeBit(c8);
                colors[BLACK].removeBit(d8);
                boards[ROOK].addBit(a8);
                boards[ROOK].removeBit(d8);
                boards[KING].addBit(e8);
                boards[KING].removeBit(c8);
                pieces[e8] = KING;
                pieces[a8] = ROOK;
                pieces[d8] = EMPTY;
                pieces[c8] = EMPTY;
                return;
            }
            assert(false);
        }
    }

    void makePromotionMove(Square from, Square to, Piece promoted, Piece captured) {
        hashSquare(hash, from);
        hashSquare(hash, to);

        colors[currentPlayer].removeBit(from);
        boards[PAWN].removeBit(from);
        pieces[from] = EMPTY;

        if (captured != EMPTY) {
            boards[captured].removeBit(to);
            colors[!currentPlayer].removeBit(to);
            materials[!currentPlayer] -= values[captured];
        }

        colors[currentPlayer].addBit(to);
        boards[promoted].addBit(to);
        pieces[to] = promoted;

        materials[currentPlayer] += values[promoted] - 1;

        hashSquare(hash, from);
        hashSquare(hash, to);
    }

    void unmakePromotionMove(Square from, Square to, Piece promotion, Piece captured) {
        colors[currentPlayer].removeBit(to);
        boards[promotion].removeBit(to);
        materials[currentPlayer] -= values[promotion] - 1;

        colors[currentPlayer].addBit(from);
        boards[PAWN].addBit(from);
        pieces[from] = PAWN;

        pieces[to] = captured;
        if (captured != EMPTY) {
            boards[captured].addBit(to);
            colors[!currentPlayer].addBit(to);
            materials[!currentPlayer] += values[captured];
        }
    }

    void makeStandardMove(Square from, Square to, Piece piece, Piece captured) {
        hashSquare(hash, from);
        hashSquare(hash, to);

        colors[currentPlayer].removeBit(from);
        boards[piece].removeBit(from);
        pieces[from] = EMPTY;

        if (captured != EMPTY) {
            boards[captured].removeBit(to);
            colors[!currentPlayer].removeBit(to);
            materials[!currentPlayer] -= values[captured];
        }

        colors[currentPlayer].addBit(to);
        boards[piece].addBit(to);
        pieces[to] = piece;

        hashSquare(hash, from);
        hashSquare(hash, to);
    }

    void unmakeStandardMove(Square from, Square to, Piece piece, Piece captured) {
        colors[currentPlayer].removeBit(to);
        boards[piece].removeBit(to);

        colors[currentPlayer].addBit(from);
        boards[piece].addBit(from);
        pieces[from] = piece;

        pieces[to] = captured;
        if (captured != EMPTY) {
            boards[captured].addBit(to);
            colors[!currentPlayer].addBit(to);
            materials[!currentPlayer] += values[captured];
        }
    }

    void makeEnPassantMove(Square from, Square to) {
        hashSquare(hash, from);
        hashSquare(hash, to);
        hashSquare(hash, getPassant());

        colors[currentPlayer].removeBit(from);
        boards[PAWN].removeBit(from);
        pieces[from] = EMPTY;

        boards[PAWN].removeBit(getPassant());
        colors[!currentPlayer].removeBit(getPassant());
        pieces[getPassant()] = EMPTY;

        colors[currentPlayer].addBit(to);
        boards[PAWN].addBit(to);
        pieces[to] = PAWN;

        materials[!currentPlayer] -= 1;

        hashSquare(hash, from);
        hashSquare(hash, to);
    }

    void unmakeEnPassantMove(Square from, Square to) {
        colors[currentPlayer].removeBit(to);
        boards[PAWN].removeBit(to);
        pieces[to] = EMPTY;

        colors[currentPlayer].addBit(from);
        boards[PAWN].addBit(from);
        pieces[from] = PAWN;

        boards[PAWN].addBit(getPassant());
        colors[!currentPlayer].addBit(getPassant());
        pieces[getPassant()] = PAWN;

        materials[!currentPlayer] += 1;
    }

    void makeMove(const Move& move) {
        fullMove = (currentPlayer == WHITE ) ? fullMove : fullMove + 1;
        Square from = move.getOrigin();
        Square to = move.getDestination();
        Piece piece = pieces[from];
        assert (piece != EMPTY);
        Piece captured = move.getType() == EN_PASSANT ? PAWN : pieces[to];
        if (move.getType() == CASTLE) makeCastleMove(from,to);
        else if (move.getType() == PROMOTION) makePromotionMove(from,to,move.getPromotion(),captured);
        else if (move.getType() == EN_PASSANT) makeEnPassantMove(from,to);
        else makeStandardMove(from,to,piece,captured);

        updateStackHash(from,to,piece,captured);
        currentPlayer ^= 1;
    }

    [[nodiscard]] int updateCastlingRights(Square from, Square to) const {
        int newRights = getCastlingRights();

        if (from == e1) newRights &= ~0b11;
        if (from == e8) newRights &= ~0b1100;

        if (from == a1 | to == a1) newRights &= ~0b10;
        if (from == h1 | to == h1) newRights &= ~0b1;
        if (from == a8 | to == a8) newRights &= ~0b1000;
        if (from == h8 | to == h8) newRights &= ~0b100;

        return newRights;
    }

    void updateStackHash(Square from, Square to, Piece piece, Piece captured) {
        int halfMove = (captured != EMPTY || piece == PAWN) ? 0 : getHalfMove() + 1;

        Square newPassant = a1;
        if (piece == PAWN and abs(to - from) == 16) {
            newPassant = to;
        }
        if (getPassant() != a1) hash ^= passantHash[getPassant() % 8];
        if (newPassant != a1) hash ^= passantHash[newPassant % 8];

        hash ^= castleHash[getCastlingRights()];
        int newCastlingRights = updateCastlingRights(from,to);
        hash ^= castleHash[newCastlingRights];

        hash ^= blackHash;
        hashHistory[hashIndex++] = hash;
        stack.emplace_back(newPassant,newCastlingRights,captured,halfMove);
    }

    [[nodiscard]] bool gameIsDrawn() const {
        return this->result == 0;
    }

    void checkForGameOver() {
        int count = 0;
        if (numberLegal() == 0) {
            if (kingCheck(currentPlayer)) endGame(currentPlayer * 2 - 1);
            else endGame(0);
            return;
        }
        for (int i = 0; i < hashIndex; i++) {
            if (hashHistory[i] == hash) {
                count+=1;
            }
            if (count >= 3) {
                endGame(0);
                return;
            }
        }
        if (getHalfMove() >= 50) {
            endGame(0);
            return;
        }
        if (insufficientMaterial()) {
            endGame(0);
        }
    }

    void unmakeMove(const Move& move) {
        fullMove = (currentPlayer == WHITE ) ? fullMove - 1 : fullMove;
        Square from = move.getOrigin();
        Square to = move.getDestination();
        Piece piece = pieces[to];
        assert (pieces[to] != EMPTY);
        Piece captured = stack.back().getCaptured();
        currentPlayer ^= 1;
        stack.pop_back();

        if (move.getType() == CASTLE) unmakeCastleMove(from,to);
        else if (move.getType() == PROMOTION) unmakePromotionMove(from,to,move.getPromotion(),captured);
        else if (move.getType() == EN_PASSANT) unmakeEnPassantMove(from,to);
        else unmakeStandardMove(from,to,piece,captured);

        hashIndex--;
        hash = hashHistory[hashIndex-1];
        hashHistory[hashIndex] = 0;
        result = 2;

    }
    [[nodiscard]] std::array<Piece, 64> getPieces() const {
        return this->pieces;
    }

    bool isLegalCastle(const Move& move) {
        // TODO
        if (move.getType() == CASTLE) {
            auto origin = move.getOrigin();
            auto destination = move.getDestination();
            if (origin < destination) {
                if (isSquareAttacked(origin,currentPlayer) or isSquareAttacked(Square(origin + 1),currentPlayer)
                    or isSquareAttacked(destination,currentPlayer) or pieces[origin+1] != EMPTY
                    or pieces[origin+2] != EMPTY) {
                    return false;
                    }
                return true;
            }
            if (isSquareAttacked(origin,currentPlayer) or isSquareAttacked(Square(origin - 1),currentPlayer)
                or isSquareAttacked(destination,currentPlayer) or pieces[origin-1] != EMPTY
                or pieces[origin-2] != EMPTY or pieces[origin-3] != EMPTY) {
                return false;
                }
        }
        return true;
    }

    bool isLegal(const Move& move) {
        // TODO
        if (kingCheck(currentPlayer ^ 1)) {
            return false;
        }
        return true;
    }

    [[nodiscard]] std::array<Bitboard, 2>& getColors() {
        return this->colors;
    }
};
