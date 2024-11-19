#include "src/uci.hpp"
#include <chrono>

int main() {
    while (true) {
        std::string command;
        getline(std::cin, command);
        if (!UCI::runCommand(command)) {
            break;
        }
    }

    return 0;
}
