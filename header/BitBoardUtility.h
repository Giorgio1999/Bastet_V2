#pragma once
#include <vector>
#include <cstdint>
#include <bit>

using bitboard = uint64_t;

const bitboard ONE = (bitboard)1;
const bitboard ZERO = (bitboard)0;
extern bitboard fileMasks[8];
extern bitboard rankMasks[8];

int BitScanForwards(const bitboard &value); // returns index of least significant bit

bool CheckBit(const bitboard &value, const int &index); // Checks bit at index in 1d board

bool CheckBit(const bitboard &value, const int &i, const int &j); // Checks bit at i,j in 2d board

void SetBit(bitboard &value, const int &index); // Sets bit at index in 1d board

void SetBit(bitboard &value, const int &i, const int &j); // Sets bit at i,j in 2d board

void UnsetBit(bitboard &value, const int &index); // Unsets bit at index in 1d board

void UnsetBit(bitboard &value, const int &i, const int &j); // Unsets bit at i,j in 2d baord

void PrintBitBoard(const bitboard &value); // Prints Bitboard to std::cout

void ComputeMasks();
