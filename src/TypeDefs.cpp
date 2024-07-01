#include "TypeDefs.h"

// Declaration of lookup tables
// -----------------------------------------
bitboard fileMasks[8];
bitboard rankMasks[8];
bitboard diagonalAttackMasks[64];
bitboard antiDiagonalAttackMasks[64];
bitboard rankAttackMasks[64];
bitboard hashes[781];

bitboard knightMoves[64];
bitboard kingMoves[64];
bitboard pawnAttacks[2][2][64];
bitboard fillUpAttacks[8][64];
bitboard aFileAttacks[8][64];
// -------------------------------------------------------------------
