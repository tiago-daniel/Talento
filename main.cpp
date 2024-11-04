#include <iostream>
#include "src/movegen.hpp"

int main() {
  initAllAttackTables();
  MoveList moves;
  MoveGen::kingMove(moves, a1, 1);
  return 0;
}
