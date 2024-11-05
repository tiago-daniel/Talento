#include "src/board.hpp"
#include <chrono>

int main() {
    auto game = Board("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
    auto moves = game.pseudoLegalMoves();
    for (int i = 0; i < moves.getSize(); i++) {
        std::cout << moves.getMoves()[i] << "\n";
    }
    game.makeMove({g2,h3,NORMAL});
    moves = game.pseudoLegalMoves();
    for (int i = 0; i < moves.getSize(); i++) {
        std::cout << moves.getMoves()[i] << "\n";
    }
    return 0;
}
