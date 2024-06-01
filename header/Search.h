#pragma once
#include "Timer.h"
#include "Engine.h"

// External Functions
// --------------------------------------------------------
namespace Search
{
    move GetBestMove(Engine &engine, const Timer &timer); // Return best moves
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------
int Min(Engine &engine, int depthRemaining);                               // Min part of MinMax
int Max(Engine &engine, int depthRemaining);                               // Max part of MinMax
int AlphaBetaMin(Engine &engine, int alpha, int beta, int depthRemaining); // Alpha Beta min search
int AlphaBetaMax(Engine &engine, int alpha, int beta, int depthRemaining); // Alpha Beta max search
int QuiescenceMin(Engine &engine, int alpha, int beta);                    // Alpha Beta min, but only captures
int QuiescenceMax(Engine &engine, int alpha, int beta);                    // Alpha Beta max but only captures
int FixedDepthSearch(Engine &engine, int depth);                           // Perform negamax search to a fixed depth
int AlphaBetaSearch(Engine &engine, int alpha, int beta, int depth);       // AlphaBeta search to a fixed depth
                                                                           // --------------------------------------------------------