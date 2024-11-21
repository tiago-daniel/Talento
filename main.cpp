#include "src/uci.hpp"

int main() {
    Evaluation::init_tables();
    initAllAttackTables();
    initMagics();
    initZobrist();
    UCI::runCommands();

    return 0;
}
