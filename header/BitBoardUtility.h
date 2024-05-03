#pragma once
#include <vector>
#include <cstdint>

const uint_fast64_t ONE = (uint_fast64_t)1;
const uint_fast64_t ZERO = (uint_fast64_t)0;

bool CheckBit(const uint_fast64_t& value,const int& index);	//Checks bit at index in 1d board

bool CheckBit(const uint_fast64_t& value, const int& i, const int& j);	//Checks bit at i,j in 2d board

void SetBit(uint_fast64_t& value, const int& index);	//Sets bit at index in 1d board

void SetBit(uint_fast64_t& value, const int& i, const int& j);	//Sets bit at i,j in 2d board

void UnsetBit(uint_fast64_t& value, const int& index);	//Unsets bit at index in 1d board

void UnsetBit(uint_fast64_t& value, const int& i, const int& j);	//Unsets bit at i,j in 2d baord

void PrintBitBoard(const uint_fast64_t& value); //Prints Bitboard to std::cout