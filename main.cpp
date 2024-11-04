#include "src/movegen.hpp"
#include <chrono>

int main() {
  initAllAttackTables();
  initMagics();
  MoveList moves;
  MoveGen::pawnMove(moves,WHITE,a7, Bit(a7+8), Bit(a8+9));
  for (int i = 0; i < moves.getSize(); i++) {
    std::cout << moves.getMoves()[i] << "\n";
  }
  return 0;
}
