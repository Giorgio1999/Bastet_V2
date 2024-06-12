#include "Search.h"
#include "Timer.h"
#include "Engine.h"
#include "Evaluation.h"
#include "MathUtility.h"
#include <algorithm>
#include <vector>
#include <iostream>

// External Functions
// --------------------------------------------------------
move Search::GetBestMove(Engine &engine, Timer &timer)
{
    Board &currentBoard = engine.CurrentBoard();
    auto color = (currentBoard.flags & 1) == 1;

    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

    std::array<int, 256> scores;
    // int alpha = -INT32_MAX; // Initial lower bound
    // int beta = INT32_MAX;   // Initial upper bound
    if (color)
    {
        for (uint i = 0; i < moveHolderIndex; i++)
        {
            engine.MakeMove(moveHolder[i]);
            scores[i] = AlphaBetaMin(engine, 0, 0, 4);
            engine.UndoLastMove();

            // if (scores[i] > alpha) // If the obtained score is higher than the lower bound, we can update the lower bound to reflect the fact that we are guaranteed this score by making this move
            // {
            //     alpha = scores[i];
            // }
        }
    }
    else
    {
        for (uint i = 0; i < moveHolderIndex; i++)
        {
            engine.MakeMove(moveHolder[i]);
            scores[i] = AlphaBetaMax(engine, 0, 0, 4);
            engine.UndoLastMove();

            // if (scores[i] < beta)   // Black will try to lower the upper bound of the score; so if the current move results in a lower score than the current upper bound, update it
            // {
            //     beta = scores[i];
            // }
        }
    }

    MathUtility::Sort<move, int, 256>(moveHolder, scores, moveHolderIndex, color);
    std::cout << "info eval " << scores[0] / (float)pieceValues[0] << std::endl;

    return moveHolder[0];
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------

int AlphaBetaMin(Engine &engine, int alpha, int beta, int depthRemaining)
{
    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2 || engine.CurrentBoard().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
    {
        return 0;
    }

    if (depthRemaining == 0) // If target depth is reached, return static evaluation of the position
    {
        return Evaluation::StaticEvaluation(engine);
    }

    // Get Legal moves
    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

    if (!engine.IsCheck() && moveHolderIndex == 0)
    { // Stalemate -> return 0
        std::cout << std::endl;
        return 0;
    }

    int bestScore = INT32_MAX;                 // Worst possible score for the minimizing player
    for (uint i = 0; i < moveHolderIndex; i++) // Check every move i can make and see if i can improve the score
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = AlphaBetaMax(engine, 0, 0, depthRemaining - 1);
        engine.UndoLastMove();

        if (tmpScore < bestScore) // Find the best move i can make in this position
        {
            bestScore = tmpScore;
        }
    }
    return bestScore;
}

int AlphaBetaMax(Engine &engine, int alpha, int beta, int depthRemaining)
{
    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2 || engine.CurrentBoard().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
    {
        return 0;
    }

    if (depthRemaining == 0) // If target depth is reached, return static evaluation of the position
    {
        return Evaluation::StaticEvaluation(engine);
    }

    // Get Legal moves
    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

    if (!engine.IsCheck() && moveHolderIndex == 0)
    { // Stalemate -> return 0
        return 0;
    }

    int bestScore = -INT32_MAX;                // Worst possible score for the maximizing player
    for (uint i = 0; i < moveHolderIndex; i++) // Check every move i can make and see if i can improve the score
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = AlphaBetaMin(engine, 0, 0, depthRemaining - 1);
        engine.UndoLastMove();

        if (tmpScore > bestScore) // Find the best move i can make in this position
        {
            bestScore = tmpScore;
        }
    }
    return bestScore;
}

int QuiescenceMin(Engine &engine, int alpha, int beta)
{

    return alpha;
}

int QuiescenceMax(Engine &engine, int alpha, int beta)
{

    return beta;
}

// --------------------------------------------------------