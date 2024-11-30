#include "src/uci.hpp"

int main() {
    Evaluation::init_tables();
    initAllAttackTables();
    initMagics();
    initZobrist();
    UCI uci{};
    uci.runCommands();

    return 0;
}
