#include "src/search.hpp"
#include <chrono>
#include <unistd.h>

int main() {
    auto game = Board();
    Evaluation::init_tables();
    /*
    auto start = std::chrono::high_resolution_clock::now();
    auto nodes = Search::perft(game, 6);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Nodes : " << nodes << std::endl;
    std::cout << "Time taken : " << duration.count() / 1000.0 << "seconds" << std::endl;
    std::cout << "MNps : " << (nodes / 1000 / duration.count())  << std::endl;
    */
    while (game.getResult() == 2) {
        auto moves = game.allMoves().getMoves();
        for (int i = 0; i < game.allMoves().getSize(); i++) {
            std::cout << i << " : " << moves[i] << std::endl;
        }
        int n = 0;
        std::cout << "Make your move : ";
        std::cin >> n;
        std::cout << std::endl;
        game.makeMove(moves[n]);
        auto move = Search::rootNegaMax(game, 4);
        if (move == Move()) break;
        game.makeMove(move);
        std::cout << "Engine made : " << move << std::endl;
        std::cout << game.getPieces() << std::endl;
    }
    sleep(5);
    return 0;
}
