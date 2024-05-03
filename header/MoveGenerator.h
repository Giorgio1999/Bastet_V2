#pragma once
#include "Engine.h"
#include "Move.h"
#include <vector>

namespace MoveGenerator{
    void GetPseudoLegalMoves(const Engine& engine, std::vector<Move>& pseudoLegalMoves);
}

void GetPseudoLegalPawnMoves(const Engine& engine, std::vector<Move>& pseudoLegalMoves);
void GetPseudoLegalKnightMoves(const Engine& engine, std::vector<Move>& pseudoLegalMoves);
