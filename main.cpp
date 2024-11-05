#include "src/movegen.hpp"
#include <chrono>

int main() {
  initAllAttackTables();
  initMagics();
  MoveList moves;
  MoveGen::passantMove(moves,WHITE,a2, b2);
  for (int i = 0; i < moves.getSize(); i++) {
    std::cout << moves.getMoves()[i] << "\n";
  }
  return 0;
}
