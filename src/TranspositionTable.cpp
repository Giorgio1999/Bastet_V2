#include "TranspositionTable.h"
#include <iostream>

transposition::Tt::Tt() {}

transposition::Tt::Tt(bitboard ttSize) {
    std::cerr << "Tt: Constructor[Tt](" << ttSize << ")" << std::endl;
    ttEntrySize = sizeof(TtEntry);
    numEntries = ttSize * 1024 * 1024 / ttEntrySize;
    std::cerr << "Tt: Calculating number of Entries" << std::endl;

    if ((numEntries & (numEntries - 1)) != 0) {
        std::cerr << "Tt: number of entries " << numEntries
                  << " not power of two." << std::endl;
        uint32_t power = 1;
        while (power <= numEntries) {
            power <<= 1;
        }
        numEntries = power >> 1;
        std::cerr << "Tt: reduce to " << numEntries << std::endl;
    }

    std::cerr << "Tt: Clear old tt" << std::endl;
    delete[] tt;
    std::cerr << "Tt: Allocate new tt" << std::endl;
    tt = new TtEntry[numEntries];

    if (tt == nullptr) {
        std::cerr << "Memory allocation failed" << std::endl;
    }
}

// transposition::Tt::~Tt()
// {
//     delete[] tt;
//     tt = nullptr;
// }

void transposition::Tt::Save(const int depthRemaining, const int score,
                             const NodeType node, const bitboard zobrist) {
    bitboard index = (numEntries - 1) & zobrist;
    TtEntry entry = tt[index];
    if (entry.depthRemaining > depthRemaining) {
        return;
    }
    if (entry.depthRemaining == -1) {
        ttFill++;
    }
    entry.zobristKey = zobrist;
    entry.depthRemaining = depthRemaining;
    entry.score = score;
    entry.nodeType = node;
    tt[index] = entry;
}

bool transposition::Tt::Pull(int &score, const int depthRemaining,
                             const int alpha, const int beta,
                             const bitboard zobrist) {
    bitboard index = (numEntries - 1) & zobrist;
    TtEntry entry = tt[index];
    if (entry.zobristKey != zobrist || entry.depthRemaining < depthRemaining) {
        return false;
    }
    switch (entry.nodeType) {
    case PV:
        score = entry.score;
        return true;
        break;
    case LOWER:
        if (entry.score <= alpha) {
            score = alpha;
            return true;
        }
        break;
    case UPPER:
        if (entry.score >= beta) {
            score = beta;
            return true;
        }
        break;
    }
    return false;
}
