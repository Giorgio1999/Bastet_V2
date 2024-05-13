#include "BitBoardUtility.h"
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>
#include <bit>

bitboard fileMasks[8];
bitboard rankMasks[8];
bitboard diagonalAttackMasks[64];
bitboard antiDiagonalAttackMasks[64];
bitboard rankAttackMasks[64];

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

bool CheckBit(const line &value, const int &index)
{
	line bitIndex = 1 << index;
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

void SetBit(line &value, const int &index)
{
	line bitIndex = 1 << index;
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

move Move(const move& move, const int& convertTo, const int& promotion){
	return move | convertTo << 12 | promotion << 15;
}

move Move(const int& from, const int& to){
	return from | to << 6;
}

move Move(const int& from, const int& to, const int& convertTo, const int& promotion){
	return from | to << 6 | convertTo << 12 | promotion << 15;
}

int StartIndex(const move& move){
	return move & 0x003F;
}

int TargetIndex(const move&move){
	return (move & 0x0FC0) >> 6;
}

int ConvertTo(const move& move){
	return (move & 0x7000) >> 12;
}

int Promotion(const move& move){
	return (move&0x8000) >> 15;
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

void PrintLine(const line &value)
{
	std::string lineVisual = "";
	for (auto i = 0; i < 8; i++)
	{
		lineVisual += std::to_string(CheckBit(value, i));
	}
	std::cout << lineVisual << std::endl;
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

	for(auto index = 0;index<64;index+=9){
		diagonalAttackMasks[index] = 0x8040201008040201;
		for(auto i = 1; i < 8 - (index&7);i++){
			diagonalAttackMasks[index+i] = (diagonalAttackMasks[index+i-1] & ~fileMasks[7]) << 1;
		}
		for(auto i=-1;i>=-(index&7);i--){
			diagonalAttackMasks[index+i] = (diagonalAttackMasks[index+i+1] & ~fileMasks[0]) >> 1;
		}
	}

	for(auto index = 7;index<57;index+=7){
		antiDiagonalAttackMasks[index] = 0x0102040810204080;
		for(auto i=	1;i<8-(index&7);i++){
			antiDiagonalAttackMasks[index+i] = (antiDiagonalAttackMasks[index+i-1] & ~fileMasks[7]) << 1;
		}
		for(auto i=-1;i>=-(index&7);i--){
			antiDiagonalAttackMasks[index+i] = (antiDiagonalAttackMasks[index+i+1] & ~fileMasks[0]) >> 1;
		}
	}

	for(auto index = 0;index<64;index+=8){
	rankAttackMasks[index] = rankMasks[7] << index;
		for(auto shift = 0;shift<8;shift++){
			rankAttackMasks[index+shift] = rankAttackMasks[index];
		}
	}
}