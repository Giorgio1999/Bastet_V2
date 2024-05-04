#pragma once
#include "Engine.h"
#include "Move.h"
#include <vector>

namespace MoveGenerator
{
    void GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
}

void GetPseudoLegalPawnMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalKnightMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalKingMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalRookMoves(const Engine &engine, const uint_fast64_t &rookPieceBoard, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalBishopMoves(const Engine &engine, const uint_fast64_t &bishopPieceBoard, std::vector<Move> &pseudoLegalMoves);