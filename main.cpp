#include "src/search.hpp"
#include <chrono>
#include <unistd.h>

int main() {
    auto game = Board("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    auto start = std::chrono::high_resolution_clock::now();
    auto nodes = Search::perft(game, 5);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Nodes : " << nodes << std::endl;
    std::cout << "Time taken : " << duration.count() / 1000.0 << "seconds" << std::endl;
    std::cout << "MNps : " << (nodes / 1000 / duration.count())  << std::endl;

    sleep(5);
    return 0;
}
