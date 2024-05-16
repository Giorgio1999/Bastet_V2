#pragma once
#include <vector>
#include <cstdint>
#include <bit>

using bitboard = uint64_t;
using line = uint8_t;
using move = uint16_t;
using flag = uint8_t;
using square = uint_fast8_t;

const bitboard ONE = (bitboard)1;
const bitboard ZERO = (bitboard)0;
extern bitboard fileMasks[8];
extern bitboard rankMasks[8];
extern bitboard diagonalAttackMasks[64];
extern bitboard antiDiagonalAttackMasks[64];
extern bitboard rankAttackMasks[64];

int BitScanForwards(const bitboard &value); // returns index of least significant bit

int PopLsb(bitboard &value); // returns index of least significant bit and removes it

bool CheckBit(const bitboard &value, const int &index); // Checks bit at index in 1d board

bool CheckBit(const line &value, const int &index); // Checks bit in file

bool CheckBit(const bitboard &value, const int &i, const int &j); // Checks bit at i,j in 2d board

void SetBit(bitboard &value, const int &index); // Sets bit at index in 1d board

void SetBit(line &value, const int &index); // Sets bit in line

void SetBit(bitboard &value, const int &i, const int &j); // Sets bit at i,j in 2d board

void UnsetBit(bitboard &value, const int &index); // Unsets bit at index in 1d board

void UnsetBit(bitboard &value, const int &i, const int &j); // Unsets bit at i,j in 2d baord

move Move(const move& move,const int& convertTo, const int & promotion);

move Move(const int& from, const int& to);

move Move(const int& from, const int& to, const int& convertTo, const int& promotion);

int StartIndex(const move& move);

int TargetIndex(const move& move);

int ConvertTo(const move& move);

int Promotion(const move& move);

void PrintBitBoard(const bitboard &value); // Prints Bitboard to std::cout

void PrintLine(const line &value); // Prints line to std::cout

void ComputeMasks();

