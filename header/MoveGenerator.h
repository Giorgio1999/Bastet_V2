#pragma once
#include "BitBoardUtility.h"
#include "Engine.h"
#include "Move.h"
#include <vector>

extern bitboard knightMoves[64];
extern bitboard kingMoves[64];
extern bitboard pawnAttacks[2][2][64]; // color,direction,index
extern bitboard fillUpAttacks[8][64];  // fill up attacks for kindergarten bitboards: file,occupation, contains bitboards consisting of 8 copies of first rank attacks for the given file and occupation
extern bitboard aFileAttacks[8][64];    // attacks on the a file indexed by, rank,occupation
                                       // white kingside    whitequeenside      black kingside      black queenside
const bitboard castleMasks[2][2] = {{0x6000000000000000, 0x0E00000000000000}, {0x0000000000000060, 0x000000000000000E}};

namespace MoveGenerator
{
    void GetPseudoLegalMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
    void GetLegalMoves(Engine &engine, std::vector<Move> &legalMoves);
    bool IsSquareAttacked(const Engine &engine, const Coord &square, const bool &attackingColor);
    bool IsSquareAttacked(const Engine &engine, const int &index, const bool &attackingColor);
    void PreComputeMoves();
}

void PreComputeKnightMoves();
void PreComputeKingMoves();
void PreComputePawnAttacks();
void PreComputeFillUpAttacks();
void PreComputeAFileAttacks();

bitboard GetDiagonalAttacks(const int &index, const bitboard &occ);
bitboard GetAntiDiagonalAttacks(const int &index, const bitboard &occ);
bitboard GetRankAttacks(const int &index, const bitboard &occ);

void GetPseudoLegalPawnMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalKnightMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalKingMoves(const Engine &engine, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalRookMoves(const Engine &engine, const bitboard &rookPieceBoard, std::vector<Move> &pseudoLegalMoves);
void GetPseudoLegalBishopMoves(const Engine &engine, const bitboard &bishopPieceBoard, std::vector<Move> &pseudoLegalMoves);
