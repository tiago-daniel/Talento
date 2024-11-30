//
// Created by metta on 11/30/24.
//

#ifndef TT_HPP
#define TT_HPP

#include "utils.hpp"

struct TTEntry {
    uint64 zobrist = 0;
    uint16 actualMove = 0;
};


class TT {
    std::vector<TTEntry> transpositionTable;
public:

    TT() {
        resize(32);
    }

    explicit TT(const int64 hashSize = 32) {
        resize(hashSize);
    }

    TTEntry& entry(const uint64 zobristHash) {
        return transpositionTable[index(zobristHash)];
    }

    void resize(int64 hashSize) {
        hashSize = std::clamp(hashSize, static_cast<int64>(1), static_cast<int64>(1048576));
        uint64 numEntries = (uint64)hashSize * 1024 * 1024 / sizeof(TTEntry);

        transpositionTable.clear();
        transpositionTable.resize(numEntries);
        transpositionTable.shrink_to_fit();
    }

    void clear() {
        transpositionTable.clear();
        transpositionTable.resize(transpositionTable.size());
        transpositionTable.shrink_to_fit();
    }

    void update(const uint64 zobrist, const uint16 move) {
        transpositionTable[index(zobrist)].actualMove = move;
        transpositionTable[index(zobrist)].zobrist = zobrist;
    }

    uint64 index(const uint64 zobristHash){
        return ((uint128)zobristHash * (uint128)transpositionTable.size()) >> 64;
    }

    void size() {
        std::cout << "info string TT size " << round(uint64(transpositionTable.size()) * sizeof(TTEntry) / 1024 / 1024)
                << " MB"
                << " (" << transpositionTable.size() << " entries)"
                << std::endl;
    }
};



#endif //TT_HPP
