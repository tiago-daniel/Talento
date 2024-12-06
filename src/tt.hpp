//
// Created by metta on 11/30/24.
//

#ifndef TT_HPP
#define TT_HPP

#include "utils.hpp"

enum Bound {
    NONE,
    EXACT,
    LOWER,
    UPPER
};

struct TTEntry {
    uint64 zobrist = 0;
    uint16 actualMove = 0;
    uint16 depthBound = 0;
    int score = 0;

    [[nodiscard]] uint16 depth() const{
        return (depthBound & 0b1111111);
    }

    [[nodiscard]] Bound bound() const{
        return Bound((depthBound >> 7) & 0b11);
    }

    void adjustScore(const uint16 ply) {
        if (score <= - mateScore) {
            score += ply;
        }
        else if (score >= mateScore) {
            score -= ply;
        }
    }

    void update(const uint64 newZobrist, const uint16 newMove, const Bound newBound, const uint16 newDepth,
        const int newScore, const int ply) {
            if (this->zobrist != zobrist
                || newBound == EXACT
                || this->depth() < newDepth){
                    this->zobrist = newZobrist;

                    this->depthBound = newDepth;
                    this->depthBound |= (uint16)newBound << 7;

                    if (newScore >= mateScore) {
                        this->score = newScore + ply;
                    }
                    else if (newScore <= - mateScore) {
                        this->score = newScore - ply;
                    }
                    else {
                        this->score = newScore;
                    }

                    if (this->zobrist != newZobrist
                        || this->actualMove == 0
                        || newBound != UPPER) {
                        this->actualMove = newMove;
                    }
                }
        }
};


class TT {
private:
    std::vector<TTEntry> transpositionTable;
public:

    TT() {
        resize(32);
    }

    explicit TT(const int64 hashSize = 32) {
        resize(hashSize);
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

    uint64 index(const uint64 zobristHash) const {
        return ((uint128)zobristHash * (uint128)transpositionTable.size()) >> 64;
    }

    void size() {
        std::cout << "info string TT size " << round(uint64(transpositionTable.size()) * sizeof(TTEntry) / 1024 / 1024)
                << " MB"
                << " (" << transpositionTable.size() << " entries)"
                << std::endl;
    }

    TTEntry operator[](const uint64 hash) const{
        return transpositionTable[index(hash)];
    }

    TTEntry& operator[](const uint64 hash){
        return transpositionTable[index(hash)];
    }
};



#endif //TT_HPP
