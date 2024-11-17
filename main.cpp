#include "src/search.hpp"
#include <chrono>
#include <unistd.h>

int main() {
    auto game = Board("rnbqkbnr/pppppppp/8/8/8/P7/P1PPPPPP/RNBQKBNR w KQkq - 0 1");
    auto start = std::chrono::high_resolution_clock::now();
    auto nodes = Search::perft(game, 6);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Nodes : " << nodes << std::endl;
    std::cout << "Time taken : " << duration.count() / 1000.0 << "seconds" << std::endl;
    std::cout << "MNps : " << (nodes / 1000 / duration.count())  << std::endl;

    sleep(5);
    return 0;
}
