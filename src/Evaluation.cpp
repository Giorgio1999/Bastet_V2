#include "Engine.h"
#include "Evaluation.h"
#include <iostream>

int Evaluation::StaticEvaluation(Engine &engine)
{
    int evaluation = 0;
    auto color = (engine.gameHistory[engine.gameHistoryIndex].flags & 1) == 1;
    auto colorMultiplier = color ? 1 : -1;

    for(auto i=0;i<5;i++){
        evaluation += pieceValues[i]*colorMultiplier*(NumberOfSetBits(engine.gameHistory[engine.gameHistoryIndex].pieceBoards[i])-NumberOfSetBits(engine.gameHistory[engine.gameHistoryIndex].pieceBoards[i+6]));
    }
    std::cout << evaluation << std::endl;
    return evaluation;
}