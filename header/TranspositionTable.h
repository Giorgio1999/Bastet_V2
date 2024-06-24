#pragma once
#include "BitBoardUtility.h"

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
        bitboard zobristKey;
        int score;
        NodeType nodeType;
        int depthRemaining;
    };

    class Tt
    {
    public:
        TtEntry *tt = nullptr;
        int ttSize;
        int ttEntrySize;
        int numEntries;
        int ttFill;

        Tt();
        Tt(int ttSize);
        // ~Tt();
        void Save(int depthRemaining, int score, NodeType node, bitboard zobrist);
        bool Pull(int &score, int depthRemaining, int alpha, int beta, bitboard zobrist);
    };

}