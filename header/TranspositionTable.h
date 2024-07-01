#pragma once
#include "BitBoardUtility.h"
#include <vector>

namespace transposition
{
enum NodeType
{
    PV,
    UPPER,
    LOWER
};

struct TtEntry
{
    bitboard zobristKey = 0;
    int score = 0;
    NodeType nodeType;
    int depthRemaining = -1;
};

class Tt
{
  public:
    std::vector<TtEntry> tt;
    bitboard ttEntrySize;
    bitboard numEntries;
    bitboard ttFill = 0;

    Tt ();
    Tt (bitboard ttSize);
    // ~Tt();
    void Save (const int depthRemaining, const int score, const NodeType node,
               const bitboard zobrist);
    bool Pull (int &score, const int depthRemaining, const int alpha, const int beta,
               const bitboard zobrist);
};

} // namespace transposition
