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
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    move bestMove = moveHolder[0];
    int alpha = -INT32_MAX;                                          // Assume worst possible lower bound
    int beta = INT32_MAX;                                            // Assume best possible higher bound
    for (uint i = 0; (i < moveHolderIndex) && !engine.stopFlag; i++) // I search for the moves which brings me the highest score. To do so i play every move i have and evaluate the position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = AlphaBetaMax(engine, alpha, beta, engine.maxDepth - 1);
        engine.UndoLastMove();

        if (tmpScore > alpha)
        {
            alpha = tmpScore;
            bestMove = moveHolder[i];
        }
    }
    std::cout << "info eval " << alpha << std::endl;
    return bestMove;
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------
int Min(Engine &engine, int depthRemaining)
{
    if (depthRemaining == 0) // If the depth limit is reached I evaluate the position. However, since it is my opponents turn to move and the evaluation is from his perspective, I flip the sign
    {
        return -Evaluation::StaticEvaluation(engine);
    }

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        return 0;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);

    int bestScore = -INT32_MAX;
    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the highest score I can achieve from this position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = Max(engine, depthRemaining - 1);
        engine.UndoLastMove();
        if (tmpScore >= bestScore)
        {
            bestScore = tmpScore;
        }
    }
    return bestScore;
}

int Max(Engine &engine, int depthRemaining)
{
    if (depthRemaining == 0) // If the depth limit is reached I evaluate the position from my perspective
    {
        return Evaluation::StaticEvaluation(engine);
    }

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        return 0;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);

    int bestScore = INT32_MAX;
    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the bestmove of my opponent, meaning the lowest score from my perspective in the given position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = Min(engine, depthRemaining - 1);
        engine.UndoLastMove();

        if (tmpScore <= bestScore)
        {
            bestScore = tmpScore;
        }
    }
    return bestScore;
}

int AlphaBetaMin(Engine &engine, int alpha, int beta, int depthRemaining)
{
    if (depthRemaining == 0) // If the depth limit is reached I evaluate the position. However, since it is my opponents turn to move and the evaluation is from his perspective, I flip the sign
    {
        return -Evaluation::StaticEvaluation(engine);
    }

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        return 0;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);

    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the highest score I can achieve from this position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = AlphaBetaMax(engine, alpha, beta, depthRemaining - 1);
        engine.UndoLastMove();

        if(tmpScore >= beta){
            return beta;
        }

        if(tmpScore > alpha){
            alpha = tmpScore;
        }
    }
    return alpha;
}

int AlphaBetaMax(Engine &engine, int alpha, int beta, int depthRemaining)
{
    if (depthRemaining == 0) // If the depth limit is reached I evaluate the position from my perspective
    {
        return Evaluation::StaticEvaluation(engine);
    }

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        return 0;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);

    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the bestmove of my opponent, meaning the lowest score from my perspective in the given position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = AlphaBetaMin(engine, alpha, beta, depthRemaining - 1);
        engine.UndoLastMove();

        if (tmpScore <= alpha)
        {
            return alpha;
        }

        if (tmpScore < beta)
        {
            beta = tmpScore;
        }
    }
    return beta;
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