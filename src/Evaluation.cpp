#include "Engine.h"
#include "Evaluation.h"
#include "Board.h"
#include "BitBoardUtility.h"
#include "MathUtility.h"
#include <iostream>

// External Functions
// -------------------------------------------------------------------
// int Evaluation::StaticEvaluation(Engine &engine)
// {
//     Board &currentBoard = engine.CurrentBoard();
//     auto color = (currentBoard.flags & 1) == 1;
//     auto colorMultiplier = color ? 1 : -1;

//     int evaluation = 0;
//     for (auto i = 0; i < 5; i++)
//     {
//         evaluation += pieceValues[i] * colorMultiplier * (NumberOfSetBits(currentBoard.pieceBoards[i]) - NumberOfSetBits(currentBoard.pieceBoards[i + 6])); // Count pieces multiplied with piece Values
//     }

//     return evaluation;
// }

int Evaluation::StaticEvaluation(Engine &engine)
{
    MathUtility::Random<bitboard> prng(engine.currentZobristKey);
    Board &currentBoard = engine.CurrentBoard();
    auto colorMultiplier = (currentBoard.flags & 1) == 1 ? -1 : 1; // If white is maximizing player, then after a white move it is blacks turn. So the score will be #white-#black. If black is maximizing then its whites turn and #black-#white

    int evaluation = 0;
    for (auto i = 0; i < 5; i++)
    {
        evaluation += pieceValues[i] * colorMultiplier * (NumberOfSetBits(currentBoard.pieceBoards[i]) - NumberOfSetBits(currentBoard.pieceBoards[i + 6])); // Count pieces multiplied with piece Values
    }
    evaluation -= colorMultiplier * whiteBonus; // If white is maximizing (black to play in the current position) then add tempo bonus to the evaluation. If black is maximizing subtract tempo
    evaluation += prng.Next(0, 100);            // Always add a random jiggle to the evaluation, which emulates mobility scores
    return evaluation;
}
// -------------------------------------------------------------------