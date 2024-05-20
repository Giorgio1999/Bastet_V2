#include "Search.h"
#include "Timer.h"
#include "BitBoardUtility.h"
#include "Engine.h"
#include "Evaluation.h"

move Search::GetBestMove(Engine &engine, const Timer &timer)
{
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    std::array<int, 256> scores;
    for (uint i = 0; i < moveHolderIndex; i++)
    {
        engine.MakeMove(moveHolder[i]);
        scores[i] = -FixedDepthSearch(engine, 1);
        engine.UndoLastMove();
    }
    move bestMove = moveHolder[0];
    int bestScore = -INT32_MAX;
    for (uint i = 0; i < moveHolderIndex; i++)
    {
        if (scores[i] >= bestScore)
        {
            bestScore = scores[i];
            bestMove = moveHolder[i];
        }
    }
    return bestMove;
}

int FixedDepthSearch(Engine &engine, int depth)
{
    if (depth == engine.maxDepth || engine.stopFlag)
    {
        return Evaluation::StaticEvaluation(engine);
    }
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex);
    int score = -INT32_MAX;
    for (uint i = 0; i < moveHolderIndex; i++)
    {
        engine.MakeMove(moveHolder[i]);
        int tempScore = -FixedDepthSearch(engine, depth + 1);
        engine.UndoLastMove();
        score = tempScore > score ? tempScore : score;
    }
    return score;
}