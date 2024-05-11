#include "BitBoardUtility.h"
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>
#include <bit>

bitboard fileMasks[8];
bitboard rankMasks[8];

int BitScanForwards(const bitboard &value)
{
	// return std::countr_zero(value);
	return __builtin_ffsl(value);
}

int PopLsb(bitboard &value)
{
	auto index = BitScanForwards(value) - 1;
	UnsetBit(value, index);
	return index;
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

void ComputeMasks()
{
	fileMasks[0] = 0x0101010101010101;
	rankMasks[0] = 0xFF00000000000000;
	for (auto i = 1; i < 8; i++)
	{
		fileMasks[i] = fileMasks[0] << i;
		rankMasks[i] = rankMasks[0] >> 8 * i;
	}
}