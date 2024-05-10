#pragma once
#include "Engine.h"
#include "Move.h"
#include <vector>

extern uint_fast64_t knightMoves[64];
extern uint_fast64_t kingMoves[64];
extern uint_fast64_t pawnAttacks[2][2][64]; //color,direction,index     
                                            //white kingside    whitequeenside      black kingside      black queenside
const uint_fast64_t castleMasks[2][2] = {{0x6000000000000000,0x0E00000000000000},{0x0000000000000060,0x000000000000000E}};


namespace MoveGenerator
{
    void GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
    void GetLegalMoves(Engine &engine, std::vector<Move> &legalMoves);
    void GenerateAttacks(const Engine &engine, const bool &color, uint_fast64_t &attackBoard);
    bool IsSquareAttacked(const Engine &engine, const Coord &square, const bool &attackingColor);
    bool IsSquareAttacked(const Engine&engine,const int& index,const bool& attackingColor);
    void PreComputeKnightMoves();
    void PreComputeKingMoves();
    void PreComputePawnAttacks();
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