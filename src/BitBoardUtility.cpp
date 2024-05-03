#include "BitBoardUtility.h"
#include <vector>
#include <cstdint>

bool CheckBit(uint_fast64_t value, int index) {
	uint_fast64_t bitIndex = ONE << index;
	return (value & bitIndex) == bitIndex;
}

bool CheckBit(uint_fast64_t value, int i, int j) {
	return CheckBit(value, j * 8 + i);
}

void SetBit(uint_fast64_t& value, int index) {
	uint_fast64_t bitIndex = ONE << index;
	value |= bitIndex;
}

void SetBit(uint_fast64_t& value, int i, int j) {
	SetBit(value, j * 8 + i);
}

void UnsetBit(uint_fast64_t& value, int index) {
	uint_fast64_t bitIndex = ONE << index;
	value &= ~bitIndex;
}

void UnsetBit(uint_fast64_t& value, int i, int j) {
	UnsetBit(value, j * 8 + i);
}