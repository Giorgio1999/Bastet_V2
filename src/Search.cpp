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
    auto color = (engine.CurrentBoard().flags & 1) == 1;
    auto colorMultipliyer = color ? 1 : -1;
    auto allowedTime = 0;
    if (color)
    {
        allowedTime += timer.wTime + timer.winc;
    }
    else
    {
        allowedTime += timer.bTime + timer.binc;
    }
    allowedTime /= 100.;

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

    std::array<int, 256> scores;

    auto localDepth = 1;
    move bestMove = moveHolder[0];
    auto bestScore = -INT32_MAX;

    while (allowedTime > timer.TimeElapsed() && localDepth < engine.maxDepth)
    {
        int alpha = -INT32_MAX;                                          // Assume worst possible lower bound
        int beta = INT32_MAX;                                            // Assume best possible higher bound
        for (uint i = 0; (i < moveHolderIndex) && !engine.stopFlag; i++) // I search for the moves which brings me the highest score. To do so i play every move i have and evaluate the position
        {
            engine.MakeMove(moveHolder[i]);
            int tmpScore = AlphaBetaMax(engine, alpha, beta, localDepth - 1);
            engine.UndoLastMove();

            if (tmpScore > alpha) // The best move is the move with the highest lower bound
            {
                alpha = tmpScore;
            }
            scores[i] = alpha; // Store scores for next iteration

            if (allowedTime < timer.TimeElapsed())  // Is the allowed time is up during an iteration, look at the moves searched so far and choose the best one
            {
                MathUtility::Sort<move,int,256>(moveHolder,scores,i+1,true);
                
                std::cout << "info eval " << colorMultipliyer * scores[0] << std::endl;
                return moveHolder[0];
            }
        }

        MathUtility::Sort<move, int, 256>(moveHolder, scores, moveHolderIndex, true);   // Sort moves (and scores) so that the best move is at index 0; the next iteration will start then with the best move
        
        std::cout << "info bestmove " << Move2Str(Move2Mover(moveHolder[0])) << " eval " << colorMultipliyer * scores[0] << " depth " << localDepth << std::endl;
        localDepth++;
    }
    std::cout << "info eval " << colorMultipliyer * scores[0] << std::endl;
    return moveHolder[0];
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
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

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
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

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
        return QuiescenceMax(engine, alpha, beta);
    }

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        return 0;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

    if (!engine.IsCheck() && moveHolderIndex == 0)
    {
        return 0;
    }

    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the highest score I can achieve from this position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = AlphaBetaMax(engine, alpha, beta, depthRemaining - 1);
        engine.UndoLastMove();

        if (tmpScore >= beta) // If the move is higher than the upper bound the opponent will not allow us to play it
        {
            return beta;
        }

        if (tmpScore > alpha) // I will choose the move that will increase the lower bound the most
        {
            alpha = tmpScore;
        }
    }
    return alpha;
}

int AlphaBetaMax(Engine &engine, int alpha, int beta, int depthRemaining)
{
    if (depthRemaining == 0) // If the depth limit is reached I evaluate the position from my perspective
    {
        return QuiescenceMin(engine, alpha, beta);
    }

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2)
    {
        return 0;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

    if (!engine.IsCheck() && moveHolderIndex == 0)
    {
        return 0;
    }

    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the bestmove of my opponent, meaning the lowest score from my perspective in the given position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = AlphaBetaMin(engine, alpha, beta, depthRemaining - 1);
        engine.UndoLastMove();

        if (tmpScore <= alpha) // If the score is less or equal to the current lower bound, a different move will be better
        {
            return alpha;
        }

        if (tmpScore < beta) // Opponent will choose that move that decreases the upper bound the most
        {
            beta = tmpScore;
        }
    }
    return beta;
}

int QuiescenceMin(Engine &engine, int alpha, int beta)
{
    int standPat = Evaluation::StaticEvaluation(engine); // Evaluation of the current position, will serve as a lower bound

    if (standPat >= beta) // If the move is higher than the upper bound the opponent will not allow us to play it
    {
        return beta;
    }

    if (standPat > alpha) // I will choose the move that will increase the lower bound the most
    {
        alpha = standPat;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, true);

    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the highest score I can achieve from this position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = QuiescenceMax(engine, alpha, beta);
        engine.UndoLastMove();

        if (tmpScore >= beta) // If the move is higher than the upper bound the opponent will not allow us to play it
        {
            return beta;
        }

        if (tmpScore > alpha) // I will choose the move that will increase the lower bound the most
        {
            alpha = tmpScore;
        }
    }
    return alpha;
}

int QuiescenceMax(Engine &engine, int alpha, int beta)
{
    int standPat = -Evaluation::StaticEvaluation(engine);

    if (standPat <= alpha) // If the score is less or equal to the current lower bound, a different move will be better
    {
        return alpha;
    }

    if (standPat < beta) // Opponent will choose that move that decreases the upper bound the most
    {
        beta = standPat;
    }

    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, true);

    for (uint i = 0; i < moveHolderIndex; i++) // If the depth limit is not reached, I look for the bestmove of my opponent, meaning the lowest score from my perspective in the given position
    {
        engine.MakeMove(moveHolder[i]);
        int tmpScore = QuiescenceMin(engine, alpha, beta);
        engine.UndoLastMove();

        if (tmpScore <= alpha) // If the score is less or equal to the current lower bound, a different move will be better
        {
            return alpha;
        }

        if (tmpScore < beta) // Opponent will choose that move that decreases the upper bound the most
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
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);
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
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);
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