#include "src/search.hpp"
#include <chrono>
#include <unistd.h>

int main() {
    auto game = Board("3Q4/4K3/k7/2Q5/8/8/8/8 w - - 11 7");
    Evaluation::init_tables();
    std::cout << Evaluation::eval(game) << std::endl;
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
        auto move = Search::rootNegaMax(game, 4);
        if (move == Move()) break;
        game.makeMove(move);
        std::cout << "Engine made : " << move << std::endl;
        std::cout << game.getPieces() << std::endl;
        auto moves = game.allMoves().getMoves();
        if (moves[0] == Move()) break;
        for (int i = 0; i < game.allMoves().getSize(); i++) {
            std::cout << i << " : " << moves[i] << std::endl;
        }
        int n = 0;
        std::cout << "Make your move : ";
        std::cin >> n;
        std::cout << std::endl;
        game.makeMove(moves[n]);
    }
    sleep(5);
    return 0;
}
