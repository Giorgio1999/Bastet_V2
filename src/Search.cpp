#include "Search.h"
#include "Timer.h"
#include "Engine.h"
#include "Evaluation.h"
#include <algorithm>
#include <vector>
#include <iostream>

// External Functions
// --------------------------------------------------------
move Search::GetBestMove(Engine &engine, const Timer &timer)
{
    bool maximizingPlayer = (engine.CurrentBoard().flags & 1) == 1;
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    move bestMove = moveHolder[0];
    int bestScore = -INT32_MAX;
    for (uint i = 0; (i < moveHolderIndex) && !engine.stopFlag; i++)
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = Min(engine, engine.maxDepth - 1, maximizingPlayer);
        engine.UndoLastMove();
        if (tmpScore >= bestScore)
        {
            bestMove = moveHolder[i];
            bestScore = tmpScore;
        }
    }
    std::cout << "info eval " << bestScore*(maximizingPlayer?1:-1) << std::endl;
    return bestMove;
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------
int Min(Engine &engine, int depthRemaining, bool maximizingPlayer)
{
    if (depthRemaining == 0)
    {
        return Evaluation::StaticEvaluation(engine, maximizingPlayer);
    }
    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        // std::cout << "info draw detected" << std::endl;
        return 0;
    }
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    int bestScore = INT16_MAX;
    for (uint i = 0; i < moveHolderIndex; i++)
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = Max(engine, depthRemaining - 1, maximizingPlayer);
        engine.UndoLastMove();
        if (tmpScore <= bestScore)
        {
            bestScore = tmpScore;
        }
    }
    return bestScore;
}
int Max(Engine &engine, int depthRemaining, bool maximizingPlayer)
{
    if (depthRemaining == 0)
    {
        return Evaluation::StaticEvaluation(engine, maximizingPlayer);
    }
    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        // std::cout << "info draw detected" << std::endl;
        return 0;
    }
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    int bestScore = -INT16_MAX;
    for (uint i = 0; i < moveHolderIndex; i++)
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = Min(engine, depthRemaining - 1, maximizingPlayer);
        engine.UndoLastMove();
        if (tmpScore >= bestScore)
        {
            bestScore = tmpScore;
        }
    }
    return bestScore;
}
int FixedDepthSearch(Engine &engine, int depth)
{
    if (depth == engine.maxDepth || engine.stopFlag) // If max depth is reached or search is stopped return static evaluation
    {
        return Evaluation::StaticEvaluation(engine);
    }
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    int score = -INT32_MAX;
    for (uint i = 0; (i < moveHolderIndex) && (!engine.stopFlag); i++) // Loop over all legal moves and perform fixed depth-1 negamax search
    {
        engine.MakeMove(moveHolder[i]);
        int tempScore = -FixedDepthSearch(engine, depth + 1);
        engine.UndoLastMove();
        score = tempScore > score ? tempScore : score; // Only keep highest score
    }
    return score;
}

int AlphaBetaSearch(Engine &engine, int alpha, int beta, int depth)
{
    if (depth == engine.maxDepth || engine.stopFlag)
    {
        return Evaluation::StaticEvaluation(engine);
    }
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    for (uint i = 0; (i < moveHolderIndex) && (!engine.stopFlag); i++)
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = -AlphaBetaSearch(engine, -beta, -alpha, depth + 1);
        engine.UndoLastMove();
        if (tmpScore >= beta)
        {
            return beta;
        }
        if (tmpScore > alpha)
        {
            alpha = tmpScore;
        }
    }
    return alpha;
}
// --------------------------------------------------------