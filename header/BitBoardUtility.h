#pragma once
#include <cstdint>

// Declaration of helpful types
// ----------------------------------------
using bitboard = uint64_t;  // 64 bits representing the squares of a chessboard
using line = uint8_t;   // 8 bits representing one line of the board
using move = uint16_t;  // 16 bits for representing moves (promotion:1,convertTo:3,targetIndex:6,startIndex:6)
using flag = uint8_t;   // 8 bits for bundling together different booleans 
using square = uint_fast8_t;    // 8 bits for representing a specific square
// -----------------------------------------

// Definiton of helpful constants
// -----------------------------------------
const bitboard ONE = (bitboard)1;
const bitboard ZERO = (bitboard)0;
// -----------------------------------------

// Declaration of lookup tables
// -----------------------------------------
extern bitboard fileMasks[8];
extern bitboard rankMasks[8];
extern bitboard diagonalAttackMasks[64];
extern bitboard antiDiagonalAttackMasks[64];
extern bitboard rankAttackMasks[64];
// -----------------------------------------

// Bit manipulation functions
// -----------------------------------------
int BitScanForwards(const bitboard &value); // returns index of least significant bit

int PopLsb(bitboard &value); // returns index of least significant bit and removes it

int NumberOfSetBits(bitboard &value); // Counts number of Set bits

bool CheckBit(const bitboard &value, const int &index); // Checks bit at index in 1d board

bool CheckBit(const bitboard &value, const int &i, const int &j); // Checks bit at i,j in 2d board

void SetBit(bitboard &value, const int &index); // Sets bit at index in 1d board

void SetBit(bitboard &value, const int &i, const int &j); // Sets bit at i,j in 2d board

move Move(const move &move, const int &convertTo, const int &promotion); // Add promotion properties to existing move

move Move(const int &from, const int &to); // Create regular move with from-to indices specified

move Move(const int &from, const int &to, const int &convertTo, const int &promotion); // Create move with from-to and promotion properties

int StartIndex(const move &move); // Extracts startindex from move

int TargetIndex(const move &move); // Extracts targetindex from move

int ConvertTo(const move &move); // Extracts pieceType from convert to field

int Promotion(const move &move); // Extracts promotion bool from move
// -----------------------------------------

// Visualization tools
// -----------------------------------------
void PrintBitBoard(const bitboard &value); // Prints Bitboard to std::cout
// -----------------------------------------

// Initialisation Functions for lookup tables
// -----------------------------------------
void ComputeMasks(); // Precomputes all relevant masks
// -----------------------------------------

