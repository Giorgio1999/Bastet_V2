#pragma once
#include "BitBoardUtility.h"
#include <string>
#include <vector>

// Logic for handling moves
// Datastruct for Squares
// -----------------------------------------------------------
struct Coord
{
	int x; // row
	int y; // file

	Coord();
	Coord(const int &_x, const int &_y);
};
// -----------------------------------------------------------

// Datastruct for high level move representation
// -----------------------------------------------------------
class Mover
{
public:
	int startIndex;	 // From
	int targetIndex; // To

	int convertTo = 0;		// used to indicate Promotions: 0=none,1=knight,2=bishop,3=rook,4=queen
	bool promotion = false; // distinguish promotions from none promotions

	Mover();
	// Different specialised constructors
	// -----------------------------------------------------------
	Mover(const int &start, const int &target);
	Mover(const int &i1, const int &j1, const int &i2, const int &j2);
	Mover(const Coord &_startCoord, const Coord &_targetCoord);
	// -----------------------------------------------------------
};
// -----------------------------------------------------------

// Conversion functions
// -----------------------------------------------------------
std::string Coord2Str(const Coord &coord);				// From square to string
Coord Str2Coord(const std::string &coordString);		// From string to square
Mover Str2Move(const std::string &moveString);			// From string to move
std::string Move2Str(const Mover &move);				// From move to string
std::vector<Mover> Str2Moves(std::string &movesString); // From string of moves to vector of moves
Mover Move2Mover(const move &move);						// From low level representation to high
move Mover2Move(const Mover &mover);					// From high level representation to low
														// -----------------------------------------------------------