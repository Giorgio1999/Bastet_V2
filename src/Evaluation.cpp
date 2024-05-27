#include "Engine.h"
#include "Evaluation.h"
#include "Board.h"
#include "BitBoardUtility.h"
#include <iostream>

// External Functions
// -------------------------------------------------------------------
int Evaluation::StaticEvaluation(Engine &engine)
{
    Board& currentBoard = engine.CurrentBoard();
    auto color = (currentBoard.flags & 1) == 1;
    auto colorMultiplier = color ? 1 : -1;

    int evaluation = 0;
    for(auto i=0;i<5;i++){
        evaluation += pieceValues[i]*colorMultiplier*(NumberOfSetBits(currentBoard.pieceBoards[i])-NumberOfSetBits(currentBoard.pieceBoards[i+6])); // Count pieces multiplied with piece Values
    }

    return evaluation;
}
int Evaluation::StaticEvaluation(Engine &engine,bool maximizingPlayer)
{
    Board& currentBoard = engine.CurrentBoard();
    auto colorMultiplier = maximizingPlayer ? 1 : -1;

    int evaluation = 0;
    for(auto i=0;i<5;i++){
        evaluation += pieceValues[i]*colorMultiplier*(NumberOfSetBits(currentBoard.pieceBoards[i])-NumberOfSetBits(currentBoard.pieceBoards[i+6])); // Count pieces multiplied with piece Values
    }

    return evaluation;
}
// -------------------------------------------------------------------