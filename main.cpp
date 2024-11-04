#include "src/movegen.hpp"
#include <chrono>

int main() {
  initAllAttackTables();
  initMagics();
  MoveList moves;
  MoveGen::bishopMove(moves, a1, 512, 0);
  return 0;
}
