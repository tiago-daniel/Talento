#include "src/board.hpp"
#include <chrono>

int main() {
    auto game = Board("r3k2r/p2pq1P1/bn2pnp1/2pPN3/1p2Pp2/2N2Q1p/PP1BBPPP/R3K2R w KQkq c6 0 1");
    auto moves = game.pseudoLegalMoves();
    for (int i = 0; i < moves.getSize(); i++) {
        if (moves.getMoves()[i].getType() == EN_PASSANT) {
            game.makeMove(moves.getMoves()[i]);
            game.unmakeMove(moves.getMoves()[i]);
        }
    }
    moves = game.pseudoLegalMoves();
    for (int i = 0; i < moves.getSize(); i++) {
        std::cout << moves.getMoves()[i] << "\n";
    }
    return 0;
}
