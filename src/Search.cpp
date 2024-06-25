#include "Search.h"
#include "Timer.h"
#include "Evaluation.h"
#include "MathUtility.h"
#include "TranspositionTable.h"
#include "Engine.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <cmath>

// External Functions
// --------------------------------------------------------
move Search::GetBestMove(Engine &engine, Timer &timer)
{
    float allowedTimeFraction = 50.;

    Board &currentBoard = engine.CurrentBoard();
    auto color = (currentBoard.flags & 1) == 1;
    auto colorMultiplier = color ? 1 : -1;

    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

    std::array<int, 256> scores;
    int maxdepth = 20;
    int currentDepth = 0;
    if (color)
    {
        float allowedTime = (timer.wTime + timer.winc) / allowedTimeFraction;
        while (allowedTime > timer.TimeElapsed() && currentDepth <= maxdepth)
        {
            bitboard nodes = 0;

            int alpha = -INT32_MAX; // Initial lower bound
            int beta = INT32_MAX;   // Initial upper bound

            float estimatedTimeConsumption = 0;
            for (uint i = 0; i < moveHolderIndex; i++)
            {
                if (allowedTime + estimatedTimeConsumption < timer.TimeElapsed())
                {
                    moveHolderIndex = i;
                    break;
                }

                auto time = timer.TimeElapsed();
                estimatedTimeConsumption = -time;

                if (time >= 1000)
                {
                    std::cout << "info currmove " << Move2Str(Move2Mover(moveHolder[i])) << " currmovenumber " << i << std::endl;
                }

                engine.MakeMove(moveHolder[i]);
                scores[i] = AlphaBetaMin(engine, alpha, beta, currentDepth, nodes);
                engine.UndoLastMove();

                if (scores[i] > alpha) // If the obtained score is higher than the lower bound, we can update the lower bound to reflect the fact that we are guaranteed this score by making this move
                {
                    alpha = scores[i];
                }

                estimatedTimeConsumption += timer.TimeElapsed();
            }

            MathUtility::Sort<move, int, 256>(moveHolder, scores, moveHolderIndex, color);
            auto time = timer.TimeElapsed();
            currentDepth++;
            float numEntries = (float)engine.tt.numEntries;
            float hashfull = engine.tt.ttFill / numEntries * 1000;
            std::cout << "info depth " << currentDepth << " score cp " << colorMultiplier * scores[0] << " time " << time << " nodes " << nodes;
            std::cout << " nps " << nodes / time * 1000 << " pv " << Move2Str(Move2Mover(moveHolder[0])) << " hashfull " << std::round(hashfull) << std::endl;
        }
    }
    else
    {
        float allowedTime = (timer.bTime + timer.binc) / allowedTimeFraction;
        while (allowedTime > timer.TimeElapsed() && currentDepth <= maxdepth)
        {
            bitboard nodes = 0;

            int alpha = -INT32_MAX; // Initial lower bound
            int beta = INT32_MAX;   // Initial upper bound

            float estimatedTimeConsumption = 0;

            for (uint i = 0; i < moveHolderIndex; i++)
            {
                if (allowedTime + estimatedTimeConsumption < timer.TimeElapsed())
                {
                    moveHolderIndex = i;
                    break;
                }

                auto time = timer.TimeElapsed();
                estimatedTimeConsumption = -time;

                if (time >= 1000)
                {
                    std::cout << "info currmove " << Move2Str(Move2Mover(moveHolder[i])) << " currmovenumber " << i << std::endl;
                }

                engine.MakeMove(moveHolder[i]);
                scores[i] = AlphaBetaMax(engine, alpha, beta, currentDepth, nodes);
                engine.UndoLastMove();

                if (scores[i] < beta) // Black will try to lower the upper bound of the score; so if the current move results in a lower score than the current upper bound, update it
                {
                    beta = scores[i];
                }

                estimatedTimeConsumption += timer.TimeElapsed();
            }

            MathUtility::Sort<move, int, 256>(moveHolder, scores, moveHolderIndex, color);
            auto time = timer.TimeElapsed();
            currentDepth++;
            float numEntries = (float)engine.tt.numEntries;
            float hashfull = engine.tt.ttFill / numEntries * 1000;
            std::cout << "info depth " << currentDepth << " score cp " << colorMultiplier * scores[0] << " time " << time << " nodes " << nodes;
            std::cout << " nps " << nodes / time * 1000 << " pv " << Move2Str(Move2Mover(moveHolder[0])) << " hashfull " << std::round(hashfull) << std::endl;
        }
    }

    return moveHolder[0];
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------

int AlphaBetaMin(Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes)
{
    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2 || engine.CurrentBoard().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
    {
        nodes++;
        return 0;
    }

    auto isCheck = engine.IsCheck();

    if (isCheck)
    {
        depthRemaining++;
    }

    if (!engine.tt.Pull(beta, depthRemaining, alpha, beta, engine.currentZobristKey))
    {

        if (depthRemaining == 0) // If target depth is reached, return static evaluation of the position
        {
            int score = QuiescenceMin(engine, alpha, beta, nodes);
            engine.tt.Save(0, score, transposition::PV, engine.currentZobristKey);
            return score;
        }

        // Get Legal moves
        uint moveHolderIndex = 0;
        std::array<move, 256> moveHolder;
        engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

        if (!isCheck && moveHolderIndex == 0)
        { // Stalemate -> return 0
            nodes++;
            engine.tt.Save(depthRemaining, 0, transposition::PV, engine.currentZobristKey);
            return 0;
        }

        for (uint i = 0; i < moveHolderIndex; i++) // Check every move i can make and see if i can improve the score
        {
            engine.MakeMove(moveHolder[i]);
            int tmpScore = AlphaBetaMax(engine, alpha, beta, depthRemaining - 1, nodes);
            engine.UndoLastMove();

            if (tmpScore <= alpha) // If the minimizing players move leads to a score lower than the lower bound, we can stop considering these branches, since the maximizing player will choose the path corresponding to the lower bound
            {
                nodes++;
                engine.tt.Save(depthRemaining, alpha, transposition::LOWER, engine.currentZobristKey);
                return alpha;
            }

            if (tmpScore < beta) // The minimizing player will choose the move that lowers the upper bound the most
            {
                beta = tmpScore;
            }
        }
        engine.tt.Save(depthRemaining, beta, transposition::PV, engine.currentZobristKey);
    }
    nodes++;
    return beta;
}

int AlphaBetaMax(Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes)
{
    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2 || engine.CurrentBoard().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
    {
        nodes++;
        return 0;
    }

    auto isCheck = engine.IsCheck();

    if (isCheck)
    {
        depthRemaining++;
    }

    if (!engine.tt.Pull(alpha, depthRemaining, alpha, beta, engine.currentZobristKey))
    {
        if (depthRemaining == 0) // If target depth is reached, return static evaluation of the position
        {
            int score = QuiescenceMax(engine, alpha, beta, nodes);
            engine.tt.Save(0, score, transposition::PV, engine.currentZobristKey);
            return score;
        }

        // Get Legal moves
        uint moveHolderIndex = 0;
        std::array<move, 256> moveHolder;
        engine.GetLegalMoves(moveHolder, moveHolderIndex, false);

        if (!isCheck && moveHolderIndex == 0)
        { // Stalemate -> return 0
            nodes++;
            engine.tt.Save(depthRemaining, 0, transposition::PV, engine.currentZobristKey);
            return 0;
        }

        for (uint i = 0; i < moveHolderIndex; i++) // Check every move i can make and see if i can improve the score
        {
            engine.MakeMove(moveHolder[i]);
            int tmpScore = AlphaBetaMin(engine, alpha, beta, depthRemaining - 1, nodes);
            engine.UndoLastMove();

            if (tmpScore >= beta) // If the maximizing player has a move even better than the upper bound, the minimizing player will rather choose the path corresponding to that upper bound
            {
                nodes++;
                engine.tt.Save(depthRemaining - 1, beta, transposition::UPPER, engine.currentZobristKey);
                return beta;
            }

            if (tmpScore > alpha)
            { // The maximizing player will seek to increase the lower bound
                alpha = tmpScore;
            }
        }
        engine.tt.Save(depthRemaining - 1, alpha, transposition::PV, engine.currentZobristKey);
    }
    nodes++;
    return alpha;
}

int QuiescenceMin(Engine &engine, int alpha, int beta, bitboard &nodes)
{

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2 || engine.CurrentBoard().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
    {
        nodes++;
        return 0;
    }

    int standPat = Evaluation::StaticEvaluation(engine);

    if (standPat <= alpha) // If the score is already lower than the lower bound, the maximizing player will choose a different path
    {
        nodes++;
        engine.tt.Save(0, alpha, transposition::LOWER, engine.currentZobristKey);
        return alpha;
    }

    if (standPat < beta) // standPat will serve as the upper bound
    {
        beta = standPat;
    }

    if (!engine.tt.Pull(beta, 0, alpha, beta, engine.currentZobristKey))
    {
        uint moveHolderIndex = 0;
        std::array<move, 256> moveHolder;
        engine.GetLegalMoves(moveHolder, moveHolderIndex, true);

        for (uint i = 0; i < moveHolderIndex; i++)
        {
            engine.MakeMove(moveHolder[i]);
            int tmpScore = QuiescenceMax(engine, alpha, beta, nodes);
            engine.UndoLastMove();

            if (tmpScore <= alpha) // If the minimizing players move leads to a score lower than the lower bound, we can stop considering these branches, since the maximizing player will choose the path corresponding to the lower bound
            {
                nodes++;
                engine.tt.Save(0, alpha, transposition::LOWER, engine.currentZobristKey);
                return alpha;
            }

            if (tmpScore < beta) // The minimizing player will choose the move that lowers the upper bound the most
            {
                beta = tmpScore;
            }
        }
        engine.tt.Save(0, beta, transposition::PV, engine.currentZobristKey);
    }
    nodes++;
    return beta;
}

int QuiescenceMax(Engine &engine, int alpha, int beta, bitboard &nodes)
{

    if (std::count(engine.repetitionTable.begin(), engine.repetitionTable.end(), engine.currentZobristKey) >= 2 || engine.CurrentBoard().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
    {
        nodes++;
        return 0;
    }

    int standPat = Evaluation::StaticEvaluation(engine);

    if (standPat >= beta) // If the score is already higher than the upper bound, the minimizing player will choose a different path
    {
        nodes++;
        engine.tt.Save(0, beta, transposition::UPPER, engine.currentZobristKey);
        return beta;
    }

    if (standPat > alpha) // standPat will serve as the lower bound
    {
        alpha = standPat;
    }

    if (!engine.tt.Pull(alpha, 0, alpha, beta, engine.currentZobristKey))
    {

        uint moveHolderIndex = 0;
        std::array<move, 256> moveHolder;
        engine.GetLegalMoves(moveHolder, moveHolderIndex, true);

        for (uint i = 0; i < moveHolderIndex; i++)
        {
            engine.MakeMove(moveHolder[i]);
            int tmpScore = QuiescenceMax(engine, alpha, beta, nodes);
            engine.UndoLastMove();

            if (tmpScore >= beta) // If the maximizing players move leads to a score higher than the upper bound, we can stop considering these branches, since the minimizing player will choose the path corresponding to the upper bound
            {
                nodes++;
                engine.tt.Save(0, beta, transposition::UPPER, engine.currentZobristKey);
                return beta;
            }

            if (tmpScore > alpha) // The maximizing player will choose the move that raises the lower bound the most
            {
                alpha = tmpScore;
            }
        }
        engine.tt.Save(0, alpha, transposition::PV, engine.currentZobristKey);
    }
    nodes++;
    return alpha;
}

// --------------------------------------------------------