#include "src/search.hpp"
#include <chrono>
#include <unistd.h>

int main() {
    auto game = Board("r1bqk2r/p1p1nppp/1p6/3p4/4p3/BP2P3/P1PP1PPP/R2QKBNR w - - 0 0");
    auto start = std::chrono::high_resolution_clock::now();
    auto nodes = Search::perft(game, 6);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Nodes : " << nodes << std::endl;
    std::cout << "Time taken : " << duration.count() / 1000.0 << "seconds" << std::endl;
    std::cout << "MNps : " << (nodes / 1000 / duration.count())  << std::endl;
    /*
    while (game.getResult() == 2) {
        auto move = Search::rootNegaMax(game, 4);
        game.makeMove(move);
        std::cout << "Engine made : " << move << std::endl;
        auto moves = game.allMoves().getMoves();
        for (int i = 0; i < game.allMoves().getSize(); i++) {
            std::cout << i << " : " << moves[i] << std::endl;
        }
        int n = 0;
        std::cout << "Make your move : ";
        std::cin >> n;
        std::cout << std::endl;
        game.makeMove(moves[n]);
        std::cout << game.getHash() << std::endl;
    }
    */
    sleep(5);
    return 0;
}
