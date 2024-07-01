#pragma once
#include "BitBoardUtility.h"
#include "Engine.h"
#include <array>

// External Functions
// -------------------------------------------------------------------
namespace MoveGenerator {
void GetLegalMoves(
    Engine &engine, std::array<move, 256> &moveHolder,
    uint &moveHolderIndex); // Fill up move holder array with legal moves in
                            // position, filled up to moveHolderIndex
void GetLegalCaptures(
    Engine &engine, std::array<move, 256> &moveHolder,
    uint &moveHolderIndex); // Fill up move holder array with legal captures in
                            // position, filled up to moveHolderIndex
bool IsSquareAttacked(
    Engine &engine, const int &index,
    const bool &attackingColor); // Test wheter a given square is attacked by
                                 // attacking color in the given position
void PreComputeMoves();          // Precompute lookup tables
} // namespace MoveGenerator
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
