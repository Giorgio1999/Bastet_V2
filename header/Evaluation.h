#pragma once
#include "Engine.h"

// Evaluation framework
// Evaluation parameters
// -------------------------------------------------------------------
const int pieceValues[5] = {100, 300, 300, 500, 900}; // Piece Values pawn,knight,bishop,rook,queen
const int whiteBonus = 50;                            // Bonus for white to consider first move advantage
// -------------------------------------------------------------------

//  External Functions
// -------------------------------------------------------------------
namespace Evaluation
{
    // int StaticEvaluation(Engine &engine);                        // Evaluates a position relative to player to move
    int StaticEvaluation(Engine &engine); // Evaluates a position relative to player to maximizing player
}
// -------------------------------------------------------------------
