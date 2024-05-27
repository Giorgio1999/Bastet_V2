#pragma once
#include "Engine.h"

// Evaluation framework
// Evaluation parameters
// -------------------------------------------------------------------
const int pieceValues[5] = {1, 3, 3, 5, 9}; // Piece Values pawn,knight,bishop,rook,queen
// -------------------------------------------------------------------

//  External Functions
// -------------------------------------------------------------------
namespace Evaluation
{
    int StaticEvaluation(Engine &engine);                        // Evaluates a position relative to player to move
    int StaticEvaluation(Engine &engine, bool maximizingPlayer); // Evaluates a position relative to player to maximizing player
}
// -------------------------------------------------------------------
