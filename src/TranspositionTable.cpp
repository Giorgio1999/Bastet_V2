#include "TranspositionTable.h"
#include "TypeDefs.h"
#include <cassert>
#include <exception>
#include <iostream>

transposition::Tt::Tt () {}

transposition::Tt::Tt (bitboard ttSize)
{
    CERR << "Tt: Constructor[Tt](" << ttSize << ")" << std::endl;
    ttEntrySize = sizeof (TtEntry);
    numEntries = ttSize * 1024 * 1024 / ttEntrySize;
    CERR << "Tt: Calculating number of Entries" << std::endl;

    if ((numEntries & (numEntries - 1)) != 0)
        {
            CERR << "Tt: number of entries " << numEntries << " not power of two." << std::endl;
            uint32_t power = 1;
            while (power <= numEntries)
                {
                    power <<= 1;
                }
            numEntries = power >> 1;
            CERR << "Tt: reduce to " << numEntries << std::endl;
        }

    CERR << "Tt: Clear old tt" << std::endl;
    tt.clear ();
    CERR << "Tt: Allocate new tt" << std::endl;
    try
        {
            tt.resize (numEntries);
        }
    catch (std::runtime_error &e)
        {
            CERR << "Runtime Error: " << e.what () << std::endl;
        }
    catch (std::exception &e)
        {
            CERR << "Exception: " << e.what () << std::endl;
        }

    CERR << "Tt: Allocation successful" << std::endl;
}

// transposition::Tt::~Tt()
// {
//     delete[] tt;
//     tt = nullptr;
// }

void
transposition::Tt::Save (const int depthRemaining, const int score, const NodeType node,
                         const bitboard zobrist)
{
    bitboard index = (numEntries - 1) & zobrist;
    TtEntry entry = tt.at (index);
    if (entry.depthRemaining > depthRemaining)
        {
            return;
        }
    if (entry.depthRemaining == -1)
        {
            ttFill++;
        }
    entry.zobristKey = zobrist;
    entry.depthRemaining = depthRemaining;
    entry.score = score;
    entry.nodeType = node;
    tt.at (index) = entry;
}

bool
transposition::Tt::Pull (int &score, const int depthRemaining, const int alpha, const int beta,
                         const bitboard zobrist)
{
    bitboard index = (numEntries - 1) & zobrist;
    TtEntry entry = tt.at (index);
    if (entry.zobristKey != zobrist || entry.depthRemaining < depthRemaining)
        {
            return false;
        }
    switch (entry.nodeType)
        {
        case PV: // PV score
            score = entry.score;
            CERR << "Tt: PV Pull successful" << std::endl;
            return true;
            break;
        case LOWER: // If score was a lower bound and is lower than current lower bound
            if (entry.score <= alpha)
                {
                    score = alpha;
                    CERR << "Tt: LOWER Pull successful" << std::endl;
                    return true;
                }
            break;
        case UPPER: // If score was an upper bound and is higher than current upper bound
            if (entry.score >= beta)
                {
                    score = beta;
                    CERR << "Tt: UPPER Pull successful" << std::endl;
                    return true;
                }
            break;
        }
    return false;
}
