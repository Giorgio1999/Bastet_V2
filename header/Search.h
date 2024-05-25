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
int FixedDepthSearch(Engine &engine, int depth);                     // Perform negamax search to a fixed depth
int AlphaBetaSearch(Engine &engine, int alpha, int beta, int depth); // AlphaBeta search to a fixed depth
// --------------------------------------------------------