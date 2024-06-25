#include "TranspositionTable.h"
#include <iostream>

transposition::Tt::Tt()
{
}

transposition::Tt::Tt(int ttSize)
{
    ttEntrySize = sizeof(TtEntry);
    numEntries = ttSize * 1024 * 1024 / ttEntrySize;

    delete[] tt;
    tt = new TtEntry[numEntries];

    if (tt == nullptr)
    {
        std::cerr << "Memory allocation failed" << std::endl;
    }
}

// transposition::Tt::~Tt()
// {
//     delete[] tt;
//     tt = nullptr;
// }

void transposition::Tt::Save(const int depthRemaining, const int score, const NodeType node, const bitboard zobrist)
{
    int index = (numEntries - 1) & zobrist;
    TtEntry entry = tt[index];
    if (entry.depthRemaining > depthRemaining)
    {
        return;
    }
    entry.zobristKey = zobrist;
    entry.depthRemaining = depthRemaining;
    entry.score = score;
    entry.nodeType = node;
    tt[index] = entry;
    ttFill++;
}

bool transposition::Tt::Pull(int &score, const int depthRemaining, const int alpha, const int beta, const bitboard zobrist)
{
    int index = (numEntries - 1) & zobrist;
    TtEntry entry = tt[index];
    if (entry.zobristKey != zobrist || entry.depthRemaining < depthRemaining)
    {
        return false;
    }
    switch (entry.nodeType)
    {
    case PV:
        score = entry.score;
        return true;
        break;
    case LOWER:
        if (entry.score <= alpha)
        {
            score = alpha;
            return true;
        }
        break;
    case UPPER:
        if (entry.score >= beta)
        {
            score = beta;
            return true;
        }
        break;
    }
    return false;
}