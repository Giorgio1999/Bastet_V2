#include "BitBoardUtility.h"
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>

uint_fast64_t fileMasks[8];
uint_fast64_t rankMasks[8];

bool CheckBit(const uint_fast64_t &value, const int &index)
{
	uint_fast64_t bitIndex = ONE << index;
	return (value & bitIndex) == bitIndex;
}

bool CheckBit(const uint_fast64_t &value, const int &i, const int &j)
{
	return CheckBit(value, j * 8 + i);
}

void SetBit(uint_fast64_t &value, const int &index)
{
	uint_fast64_t bitIndex = ONE << index;
	value |= bitIndex;
}

void SetBit(uint_fast64_t &value, const int &i, const int &j)
{
	SetBit(value, j * 8 + i);
}

void UnsetBit(uint_fast64_t &value, const int &index)
{
	uint_fast64_t bitIndex = ONE << index;
	value &= ~bitIndex;
}

void UnsetBit(uint_fast64_t &value, const int &i, const int &j)
{
	UnsetBit(value, j * 8 + i);
}

void PrintBitBoard(const uint_fast64_t &value)
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

void ComputeMasks(){
	fileMasks[0] = 0x0101010101010101;
	rankMasks[0] = 0xFF00000000000000;
	for(auto i=1;i<8;i++){
		fileMasks[i] = fileMasks[0] << i;
		rankMasks[i] = rankMasks[0] >> 8*i;
	}
}