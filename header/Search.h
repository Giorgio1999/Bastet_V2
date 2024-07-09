#pragma once
#include "Engine.h"
#include "Timer.h"
#include <vector>

// External Functions
// --------------------------------------------------------
namespace Search
{
move GetBestMove (Engine &engine, Timer &timer); // Return best moves
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------
int AlphaBetaMin (Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes, std::vector<move> &pv); // Alpha Beta min search
int AlphaBetaMax (Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes, std::vector<move> &pv); // Alpha Beta max search
int QuiescenceMin (Engine &engine, int alpha, int beta, bitboard &nodes, std::vector<move> &pv);                    // Alpha Beta min, but only captures
int QuiescenceMax (Engine &engine, int alpha, int beta, bitboard &nodes, std::vector<move> &pv);                    // Alpha Beta max but only captures

// --------------------------------------------------------
