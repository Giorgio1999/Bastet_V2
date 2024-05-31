#pragma once
#include "BitBoardUtility.h"
#include "Engine.h"
#include <array>

// Lookup tables declaration
// -------------------------------------------------------------------
extern bitboard knightMoves[64];       // lookup table for all knight moves indexed by: square
extern bitboard kingMoves[64];         // lookup table for all king moves indexed by: square
extern bitboard pawnAttacks[2][2][64]; // lookup table for all pawn attackes indexed by: color,direction,index
extern bitboard fillUpAttacks[8][64];  // lookup table for fill up attacks for kindergarten bitboards indexed by: file,occupation,
                                       // contains bitboards consisting of 8 copies of first rank attacks for the given file and occupation
extern bitboard aFileAttacks[8][64];   // lookup table for attacks on the a file indexed by: rank,occupation
// -------------------------------------------------------------------

// Constant masks
// -------------------------------------------------------------------
const bitboard castleMasks[2][2] = {{0x6000000000000000, 0x0E00000000000000}, {0x0000000000000060, 0x000000000000000E}}; // white kingside    whitequeenside      black kingside      black queenside
// -------------------------------------------------------------------

// External Functions
// -------------------------------------------------------------------
namespace MoveGenerator
{
    void GetLegalMoves(Engine &engine, std::array<move, 256> &moveHolder, uint &moveHolderIndex);    // Fill up move holder array with legal moves in position, filled up to moveHolderIndex
    void GetLegalCaptures(Engine &engine, std::array<move, 256> &moveHolder, uint &moveHolderIndex); // Fill up move holder array with legal captures in position, filled up to moveHolderIndex
    bool IsSquareAttacked(Engine &engine, const int &index, const bool &attackingColor);             // Test wheter a given square is attacked by attacking color in the given position
    void PreComputeMoves();                                                                          // Precompute lookup tables
}
// -------------------------------------------------------------------

// Internal Functions
// -------------------------------------------------------------------
// Precompute lookuptables
void PreComputeKnightMoves();
void PreComputeKingMoves();
void PreComputePawnAttacks();
void PreComputeFillUpAttacks();
void PreComputeAFileAttacks();

// Fetch kindergarten slider attacks
bitboard GetDiagonalAttacks(const int &index, const bitboard &occ);
bitboard GetAntiDiagonalAttacks(const int &index, const bitboard &occ);
bitboard GetRankAttacks(const int &index, const bitboard &occ);
bitboard GetFileAttacks(const int &index, const bitboard &occ);
// -------------------------------------------------------------------
