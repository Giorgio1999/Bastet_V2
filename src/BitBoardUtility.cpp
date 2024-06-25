#include "BitBoardUtility.h"
#include "MathUtility.h"
#include "TypeDefs.h"
#include <string>
#include <iostream>

// Bit manipulation functions
// -----------------------------------------
int BitScanForwards(const bitboard &value)
{
	return __builtin_ffsl(value); // built in CPU function for bitscanning
}

int PopLsb(bitboard &value)
{
	auto index = BitScanForwards(value) - 1; // Get index of LSB
	value ^= ONE << index;					 // Flip LSB off
	return index;
}

int NumberOfSetBits(bitboard &value)
{
	// Implementation of Brian Kernighan’s Algorithm
	int count = 0;
	while (value > 0)
	{
		value &= (value - 1);
		count++;
	}
	return count;
}

bool CheckBit(const bitboard &value, const int &index)
{
	bitboard bitIndex = ONE << index;
	return (value & bitIndex) == bitIndex;
}

bool CheckBit(const bitboard &value, const int &i, const int &j)
{
	return CheckBit(value, j * 8 + i);
}

void SetBit(bitboard &value, const int &index)
{
	bitboard bitIndex = ONE << index;
	value |= bitIndex;
}

void SetBit(bitboard &value, const int &i, const int &j)
{
	SetBit(value, j * 8 + i);
}

void UnsetBit(bitboard &value, const int &index)
{
	bitboard bitIndex = ONE << index;
	value &= ~bitIndex;
}

void UnsetBit(bitboard &value, const int &i, const int &j)
{
	UnsetBit(value, j * 8 + i);
}

move Move(const int &from, const int &to, const int &convertTo, const int &promotion)
{
	return from | to << 6 | convertTo << 12 | promotion << 15;
}

move Move(const int &from, const int &to)
{
	return from | to << 6;
}

move Move(const move &move, const int &convertTo, const int &promotion)
{
	return move | convertTo << 12 | promotion << 15;
}

int StartIndex(const move &move)
{
	return move & 0x003F;
}

int TargetIndex(const move &move)
{
	return (move & 0x0FC0) >> 6;
}

int ConvertTo(const move &move)
{
	return (move & 0x7000) >> 12;
}

int Promotion(const move &move)
{
	return (move & 0x8000) >> 15;
}
// -----------------------------------------

// Visualization tools
// -----------------------------------------
void PrintBitBoard(const bitboard &value)
{
	std::string boardVisual = "";
	for (auto i = 0; i < 8; i++)
	{
		for (auto j = 0; j < 8; j++)
		{
			boardVisual += std::to_string(CheckBit(value, j, i));
		}
		boardVisual += "\n";
	}
	boardVisual += "\n";
	std::cout << boardVisual;
}
// -----------------------------------------

// Initialisation Functions for lookup tables
// -----------------------------------------
void ComputeMasks()
{
	fileMasks[0] = 0x0101010101010101; // A-File mask
	rankMasks[0] = 0xFF00000000000000; // 8-Rank mask
	for (auto i = 1; i < 8; i++)
	{
		fileMasks[i] = fileMasks[0] << i;
		rankMasks[i] = rankMasks[0] >> 8 * i;
	}

	for (auto index = 0; index < 64; index += 9)
	{
		diagonalAttackMasks[index] = 0x8040201008040201; // Main diagonal mask
		for (auto i = 1; i < 8 - (index & 7); i++)
		{
			diagonalAttackMasks[index + i] = (diagonalAttackMasks[index + i - 1] & ~fileMasks[7]) << 1;
		}
		for (auto i = -1; i >= -(index & 7); i--)
		{
			diagonalAttackMasks[index + i] = (diagonalAttackMasks[index + i + 1] & ~fileMasks[0]) >> 1;
		}
	}

	for (auto index = 7; index < 57; index += 7)
	{
		antiDiagonalAttackMasks[index] = 0x0102040810204080; // main antidiagonal mask
		for (auto i = 1; i < 8 - (index & 7); i++)
		{
			antiDiagonalAttackMasks[index + i] = (antiDiagonalAttackMasks[index + i - 1] & ~fileMasks[7]) << 1;
		}
		for (auto i = -1; i >= -(index & 7); i--)
		{
			antiDiagonalAttackMasks[index + i] = (antiDiagonalAttackMasks[index + i + 1] & ~fileMasks[0]) >> 1;
		}
	}

	for (auto index = 0; index < 64; index += 8)
	{
		rankAttackMasks[index] = rankMasks[7] << index;
		for (auto shift = 0; shift < 8; shift++)
		{
			rankAttackMasks[index + shift] = rankAttackMasks[index];
		}
	}
}
void ComputeHashes()
{
	MathUtility::Random<bitboard> prng((bitboard)2938472947865982);
	for (uint i = 0; i < 781; i++)
	{
		hashes[i] = prng.Next();
	}
}
// -----------------------------------------