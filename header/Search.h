#pragma once
#include "Timer.h"
#include "Engine.h"

// External Functions
// --------------------------------------------------------
namespace Search
{
    move GetBestMove(Engine &engine, Timer &timer); // Return best moves
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------
int AlphaBetaMin(Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes); // Alpha Beta min search
int AlphaBetaMax(Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes); // Alpha Beta max search
int QuiescenceMin(Engine &engine, int alpha, int beta, bitboard &nodes);                    // Alpha Beta min, but only captures
int QuiescenceMax(Engine &engine, int alpha, int beta, bitboard &nodes);                    // Alpha Beta max but only captures
                                                                                            // --------------------------------------------------------