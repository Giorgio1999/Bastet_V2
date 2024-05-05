#pragma once
#include "Engine.h"
#include "Move.h"
#include <vector>

namespace MoveGenerator
{
    void GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
    void GetLegalMoves(Engine& engine, std::vector<Move> &legalMoves);
    void GenerateAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard);

}

void GetPseudoLegalPawnMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalKnightMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalKingMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalRookMoves(const Engine &engine, const uint_fast64_t &rookPieceBoard, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalBishopMoves(const Engine &engine, const uint_fast64_t &bishopPieceBoard, std::vector<Move> &pseudoLegalMoves);

void GeneratePawnAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard);
void GenerateKnightAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard);
void GenerateKingAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard);
void GenerateRookAttacks(const Engine &engine, const bool &color, const uint_fast64_t &rookPieceBoard, uint_fast64_t &attackBoard);
void GenerateBishopAttacks(const Engine &engine, const bool &color, const uint_fast64_t &bishopPieceBoard, uint_fast64_t &attackBoard);