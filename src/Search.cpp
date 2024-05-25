#include "Search.h"
#include "Timer.h"
#include "Engine.h"
#include "Evaluation.h"

// External Functions
// --------------------------------------------------------
move Search::GetBestMove(Engine &engine, const Timer &timer)
{
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    std::array<int, 256> scores;
    int alpha = -INT32_MAX;
    int beta = INT32_MAX;
    for (uint i = 0; (i < moveHolderIndex) && (!engine.stopFlag); i++) // Loop over all legal moves, play them on the board and perform a fixed depth search
    {
        engine.MakeMove(moveHolder[i]);
        scores[i] = -AlphaBetaSearch(engine, -beta, -alpha, 1); // Save all obtained scores via negamax search
        engine.UndoLastMove();
    }

    move bestMove = moveHolder[0];
    int bestScore = -INT32_MAX;
    for (uint i = 0; i < moveHolderIndex; i++) // Loop over all scored moves and return the move with the highest evaluation
    {
        if (scores[i] >= bestScore)
        {
            bestScore = scores[i];
            bestMove = moveHolder[i];
        }
    }
    return bestMove;
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------
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