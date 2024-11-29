//
// Created by metta on 11/28/24.
//

#ifndef TT_HPP
#define TT_HPP

#include "utils.hpp"

enum Bound : uint8 {
    EXACT,
    ALPHA,
    BETA
};


struct TTEntry {
    uint16 actualMove = 0;
    uint64 zobristHash = 0;

    constexpr void update(const uint64 newZobristHash, const uint16 newBestMove) {
        actualMove = newBestMove;
        zobristHash = newZobristHash;
    }
};

constexpr uint64 TTEntryIndex(const uint64 zobristHash, const auto numEntries) {
    return  static_cast<uint128>(zobristHash) * static_cast<uint128>(numEntries) >> 64;
}

inline void sizeTT(const std::vector<TTEntry> &tt) {
    const double bytes = uint64(tt.size()) * sizeof(TTEntry);
    const double megabytes = bytes / (1024.0 * 1024.0);

    std::cout << "info string TT size " << round(megabytes) << " MB"
              << " (" << tt.size() << " entries)"
              << std::endl;
}

constexpr void resizeTT(std::vector<TTEntry> &tt, int64 newSizeMB) {
    newSizeMB = std::clamp(newSizeMB, static_cast<int64>(1), static_cast<int64>(1048576));
    const uint64 numEntries = static_cast<uint64>(newSizeMB) * 1024 * 1024 / sizeof(TTEntry);

    tt.clear();
    tt.resize(numEntries);
    tt.shrink_to_fit();
}

constexpr void clearTT(std::vector<TTEntry> &tt){
    const auto numEntries = tt.size();
    tt.clear();
    tt.resize(numEntries);
    tt.shrink_to_fit();
}

#endif //TT_HPP
