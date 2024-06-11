#include "Engine.h"
#include "Evaluation.h"
#include "Board.h"
#include "BitBoardUtility.h"
#include "MathUtility.h"
#include <iostream>
#include <chrono>

// External Functions
// -------------------------------------------------------------------

int Evaluation::StaticEvaluation(Engine &engine)
{
    // MathUtility::Random<int> prng(engine.currentZobristKey);
    // MathUtility::Random<int> prng(47860237867345);
    Board &currentBoard = engine.CurrentBoard();
    auto colorMultiplier = (currentBoard.flags & 1) == 1 ? -1 : 1; // If white is maximizing player, then after a white move it is blacks turn. So the score will be #white-#black. If black is maximizing then its whites turn and #black-#white

    int evaluation = 0;
    // bitboard countBoard = ZERO;
    for (auto i = 0; i < 6; i++)
    {
        // countBoard = currentBoard.pieceBoards[i];
        // while (countBoard > 0)
        // {
        //     square square = PopLsb(countBoard);
        //     evaluation += colorMultiplier * pieceSquareTables[0][i][square];
        // }
        // countBoard = currentBoard.pieceBoards[i + 6];
        // while (countBoard > 0)
        // {
        //     square square = PopLsb(countBoard);
        //     evaluation -= colorMultiplier * pieceSquareTables[1][i][square];
        // }

        evaluation += pieceValues[i] * colorMultiplier * (NumberOfSetBits(currentBoard.pieceBoards[i]) - NumberOfSetBits(currentBoard.pieceBoards[i + 6])); // Count pieces multiplied with piece Values
    }
    // evaluation += colorMultiplier * whiteBonus; // If white is maximizing (black to play in the current position) then add tempo bonus to the evaluation. If black is maximizing subtract tempo
    // evaluation += colorMultiplier * engine.prng -> Next(-10, 10); // Always add a random jiggle to the evaluation, which emulates mobility scores
    return evaluation;
}
// -------------------------------------------------------------------