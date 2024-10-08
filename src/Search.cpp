#include "Search.h"
#include "Engine.h"
#include "Evaluation.h"
#include "MathUtility.h"
#include "Timer.h"
#include "TranspositionTable.h"
#include "BoardUtility.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

// External Functions
// --------------------------------------------------------
move
Search::GetBestMove (Engine &engine, Timer &timer)
{
    float allowedTimeFraction = 50.;

    Board &currentBoard = engine.CurrentBoard ();
    auto color = (currentBoard.flags & 1) == 1;
    auto colorMultiplier = color ? 1 : -1;

    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves (moveHolder, moveHolderIndex, false);

    if (timer.moverestriction)
        {
            uint newMoveHolderIndex = 0;
            for (Mover mover : timer.searchmoves)
                {
                    CERR << "Search: " << Move2Str (mover) << std::endl;
                    move move = Mover2Move (mover);
                    for (uint i = 0; i < moveHolderIndex; i++)
                        {
                            if (move == moveHolder[i])
                                {
                                    moveHolder[newMoveHolderIndex] = move;
                                    newMoveHolderIndex++;
                                }
                        }
                }
            moveHolderIndex = newMoveHolderIndex;
        }
    std::array<int, 256> scores;
    int maxdepth = std::min (20, timer.depth - 1);
    int currentDepth = 1;
    bitboard nodes = 0;
    std::array<std::vector<move>, 256> pvs;
    if (color)
        {
            float allowedTime = (timer.wTime + timer.winc) / allowedTimeFraction;
            while ((timer.inf || allowedTime > timer.TimeElapsed ()) && currentDepth <= maxdepth && !engine.stopFlag && nodes <= timer.nodes)
                {

                    int alpha = -INT32_MAX; // Initial lower bound
                    int beta = INT32_MAX;   // Initial upper bound

                    float estimatedTimeConsumption = 0;
                    for (uint i = 0; i < moveHolderIndex && !engine.stopFlag; i++)
                        {
                            if (!timer.inf && allowedTime + estimatedTimeConsumption < timer.TimeElapsed ())
                                {
                                    moveHolderIndex = i;
                                    break;
                                }

                            auto time = timer.TimeElapsed ();
                            estimatedTimeConsumption = -time;

                            if (time >= 1000)
                                {
                                    std::cout << "info currmove " << Move2Str (Move2Mover (moveHolder[i])) << " currmovenumber " << i << std::endl;
                                }
                            pvs[i].clear ();
                            pvs[i].push_back (moveHolder[i]);
                            engine.MakeMove (moveHolder[i]);
                            scores[i] = AlphaBetaMin (engine, alpha, beta, currentDepth - 1, nodes, pvs[i]);
                            engine.UndoLastMove ();

                            if (scores[i] > alpha) // If the obtained score is higher than the lower
                                                   // bound, we can update the lower bound to
                                                   // reflect the fact that we are guaranteed this
                                                   // score by making this move
                                {
                                    alpha = scores[i];
                                }

                            estimatedTimeConsumption += timer.TimeElapsed ();
                        }

                    MathUtility::Sort<move, std::vector<move>, int, 256> (moveHolder, pvs, scores, moveHolderIndex, color);
                    auto time = timer.TimeElapsed ();

                    float numEntries = (float)engine.tt.numEntries;
                    float hashfull = (float)engine.tt.ttFill / numEntries * 1000;

                    std::cout << "info depth " << currentDepth << " score cp " << colorMultiplier * scores[0] << " time " << time << " nodes " << nodes;
                    std::cout << " nps " << nodes / time * 1000 << " pv" << Moves2Str (pvs[0]) << "hashfull " << std::round (hashfull) << std::endl;
                    currentDepth++;
                }
        }
    else
        {
            float allowedTime = (timer.bTime + timer.binc) / allowedTimeFraction;
            while ((timer.inf || allowedTime > timer.TimeElapsed ()) && currentDepth <= maxdepth && !engine.stopFlag && nodes <= timer.nodes)
                {
                    int alpha = -INT32_MAX; // Initial lower bound
                    int beta = INT32_MAX;   // Initial upper bound

                    float estimatedTimeConsumption = 0;

                    for (uint i = 0; i < moveHolderIndex && !engine.stopFlag; i++)
                        {
                            if (!timer.inf && allowedTime + estimatedTimeConsumption < timer.TimeElapsed ())
                                {
                                    moveHolderIndex = i;
                                    break;
                                }

                            auto time = timer.TimeElapsed ();
                            estimatedTimeConsumption = -time;

                            if (time >= 1000)
                                {
                                    std::cout << "info currmove " << Move2Str (Move2Mover (moveHolder[i])) << " currmovenumber " << i << std::endl;
                                }
                            pvs[i].clear ();
                            pvs[i].push_back (moveHolder[i]);
                            engine.MakeMove (moveHolder[i]);
                            scores[i] = AlphaBetaMax (engine, alpha, beta, currentDepth - 1, nodes, pvs[i]);
                            engine.UndoLastMove ();

                            if (scores[i] < beta) // Black will try to lower the upper bound of the
                                                  // score; so if the current move results in a lower
                                                  // score than the current upper bound, update it
                                {
                                    beta = scores[i];
                                }

                            estimatedTimeConsumption += timer.TimeElapsed ();
                        }

                    MathUtility::Sort<move, std::vector<move>, int, 256> (moveHolder, pvs, scores, moveHolderIndex, color);
                    auto time = timer.TimeElapsed ();

                    float numEntries = (float)engine.tt.numEntries;
                    float hashfull = (float)engine.tt.ttFill / numEntries * 1000;

                    std::cout << "info depth " << currentDepth << " score cp " << colorMultiplier * scores[0] << " time " << time << " nodes " << nodes;
                    std::cout << " nps " << nodes / time * 1000 << " pv" << Moves2Str (pvs[0]) << "hashfull " << std::round (hashfull) << std::endl;
                    currentDepth++;
                }
        }
    return moveHolder[0];
}
// --------------------------------------------------------

// Internal Functions
// --------------------------------------------------------

int
AlphaBetaMin (Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes, std::vector<move> &pv)
{
    if (std::count (engine.repetitionTable.begin (), engine.repetitionTable.end (),
                    engine.currentZobristKey) >= 2 || engine.CurrentBoard ().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
        {
            nodes++;
            return 0;
        }

    auto isCheck = engine.IsCheck ();

    if (isCheck)
        {
            depthRemaining++;
        }

    if (depthRemaining == 0) // If target depth is reached, return static
                             // evaluation of the position
        {
            int score = QuiescenceMin (engine, alpha, beta, nodes, pv);
            return score;
        }

    int tmpScore;

    // Get Legal moves
    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves (moveHolder, moveHolderIndex, false);

    if (!isCheck && moveHolderIndex == 0)
        { // Stalemate -> return 0
            nodes++;
            return 0;
        }
    auto count = pv.size ();
    std::vector<move> tmpPv = pv;
    for (uint i = 0; i < moveHolderIndex && !engine.stopFlag; i++) // Check every move i can make and see if i can improve the score
        {
            tmpPv.push_back (moveHolder[i]);
            engine.MakeMove (moveHolder[i]);
            tmpScore = AlphaBetaMax (engine, alpha, beta, depthRemaining - 1, nodes, tmpPv);
            engine.UndoLastMove ();

            if (tmpScore <= alpha) // If the minimizing players move leads to a score lower than
                                   // the lower bound, we can stop considering these branches,
                                   // since the maximizing player will choose the path
                                   // corresponding to the lower bound
                {
                    nodes++;
                    return alpha;
                }

            if (tmpScore < beta) // The minimizing player will choose the move that
                                 // lowers the upper bound the most
                {
                    pv = tmpPv;
                    beta = tmpScore;
                }
            tmpPv.resize (count);
        }
    nodes++;
    return beta;
}

int
AlphaBetaMax (Engine &engine, int alpha, int beta, int depthRemaining, bitboard &nodes, std::vector<move> &pv)
{
    if (std::count (engine.repetitionTable.begin (), engine.repetitionTable.end (),
                    engine.currentZobristKey) >= 2 || engine.CurrentBoard ().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
        {
            nodes++;
            return 0;
        }

    auto isCheck = engine.IsCheck ();

    if (isCheck)
        {
            depthRemaining++;
        }

    if (depthRemaining == 0) // If target depth is reached, return static
                             // evaluation of the position
        {
            int score = QuiescenceMax (engine, alpha, beta, nodes, pv);
            return score;
        }

    int tmpScore;

    // Get Legal moves
    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves (moveHolder, moveHolderIndex, false);

    if (!isCheck && moveHolderIndex == 0)
        { // Stalemate -> return 0
            nodes++;
            return 0;
        }
    auto count = pv.size ();
    std::vector<move> tmpPv = pv;
    for (uint i = 0; i < moveHolderIndex && !engine.stopFlag; i++) // Check every move i can make and see if i can improve the score
        {
            tmpPv.push_back (moveHolder[i]);
            engine.MakeMove (moveHolder[i]);
            tmpScore = AlphaBetaMin (engine, alpha, beta, depthRemaining - 1, nodes, tmpPv);
            engine.UndoLastMove ();

            if (tmpScore >= beta) // If the maximizing player has a move even better than the
                                  // upper bound, the minimizing player will rather choose the
                                  // path corresponding to that upper bound
                {
                    nodes++;
                    return beta;
                }

            if (tmpScore > alpha)
                { // The maximizing player will seek to increase
                    // the lower bound
                    pv = tmpPv;
                    alpha = tmpScore;
                }
            tmpPv.resize (count);
        }
    nodes++;
    return alpha;
}

int
QuiescenceMin (Engine &engine, int alpha, int beta, bitboard &nodes, std::vector<move> &pv)
{

    if (std::count (engine.repetitionTable.begin (), engine.repetitionTable.end (),
                    engine.currentZobristKey) >= 2 || engine.CurrentBoard ().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
        {
            nodes++;
            return 0;
        }

    int standPat = Evaluation::StaticEvaluation (engine);

    if (standPat <= alpha) // If the score is already lower than the lower bound, the
                           // maximizing player will choose a different path
        {
            nodes++;
            return alpha;
        }

    if (standPat < beta) // standPat will serve as the upper bound
        {
            beta = standPat;
        }

    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves (moveHolder, moveHolderIndex, true);

    auto count = pv.size ();
    std::vector<move> tmpPv = pv;
    for (uint i = 0; i < moveHolderIndex && !engine.stopFlag; i++)
        {
            tmpPv.push_back (moveHolder[i]);
            engine.MakeMove (moveHolder[i]);
            int tmpScore = QuiescenceMax (engine, alpha, beta, nodes, tmpPv);
            engine.UndoLastMove ();

            if (tmpScore <= alpha) // If the minimizing players move leads to a score lower than
                                   // the lower bound, we can stop considering these branches,
                                   // since the maximizing player will choose the path
                                   // corresponding to the lower bound
                {
                    nodes++;
                    return alpha;
                }

            if (tmpScore < beta) // The minimizing player will choose the move that
                                 // lowers the upper bound the most
                {
                    pv = tmpPv;
                    beta = tmpScore;
                }
            tmpPv.resize (count);
        }
    nodes++;
    return beta;
}

int
QuiescenceMax (Engine &engine, int alpha, int beta, bitboard &nodes, std::vector<move> &pv)
{

    if (std::count (engine.repetitionTable.begin (), engine.repetitionTable.end (),
                    engine.currentZobristKey) >= 2 || engine.CurrentBoard ().nonReversibleMoves >= 50) // Repetition or fifty move rule -> return 0
        {
            nodes++;
            return 0;
        }

    int standPat = Evaluation::StaticEvaluation (engine);

    if (standPat >= beta) // If the score is already higher than the upper bound, the
                          // minimizing player will choose a different path
        {
            nodes++;
            return beta;
        }

    if (standPat > alpha) // standPat will serve as the lower bound
        {
            alpha = standPat;
        }

    uint moveHolderIndex = 0;
    std::array<move, 256> moveHolder;
    engine.GetLegalMoves (moveHolder, moveHolderIndex, true);

    auto count = pv.size ();
    std::vector<move> tmpPv = pv;
    for (uint i = 0; i < moveHolderIndex && !engine.stopFlag; i++)
        {
            tmpPv.push_back (moveHolder[i]);
            engine.MakeMove (moveHolder[i]);
            int tmpScore = QuiescenceMax (engine, alpha, beta, nodes, tmpPv);
            engine.UndoLastMove ();

            if (tmpScore >= beta) // If the maximizing players move leads to a score higher than
                                  // the upper bound, we can stop considering these branches,
                                  // since the minimizing player will choose the path
                                  // corresponding to the upper bound
                {
                    nodes++;
                    return beta;
                }

            if (tmpScore > alpha) // The maximizing player will choose the move that
                                  // raises the lower bound the most
                {
                    pv = tmpPv;
                    alpha = tmpScore;
                }
            tmpPv.resize (count);
        }
    nodes++;
    return alpha;
}
// --------------------------------------------------------
