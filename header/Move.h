#pragma once
#include "Piece.h"
#include <string>
#include <list>
#include <vector>

// Logic for handling moves

const std::string rows = "87654321"; // Hardcoded translations of the rows or y
const std::string cols = "abcdefgh"; // Hardcoded translations of the cols or x

struct Coord
{
	int x;
	int y;

	Coord();
	Coord(const int &_x, const int &_y);
};

struct Move
{
	int startIndex;	//From
	int targetIndex;	//To

	// PieceType convertTo = none; // used to indicate promotions, as well as using wheter the move is a king move or not for checks
	int convertTo = 0;	// used to indicate Promotions: 0=none,1=knight,2=bishop,3=rook,4=queen
	bool promotion = false;		// distinguish promotions from none promotions

	Move();
	Move(const int& start,const int&target);
	Move(const int &i1, const int &j1, const int &i2, const int &j2);
	Move(const Coord &_startCoord, const Coord &_targetCoord);
};

std::string Coord2Str(const Coord &coord); // Helper functions to convert from human readable strings to moves and coords
Coord Str2Coord(const std::string &coordString);
std::string Move2Str(const Move &move);
Move Str2Move(const std::string &moveString);
std::vector<Move> Str2Moves(std::string &movesString);
int Coord2Index(const Coord &coord);			   // convert between coord and 1d Board index
bool operator==(const Coord &lhs, const Coord &rhs); // To compare coords
bool operator!=(const Coord &lhs, const Coord &rhs); // negation
int operator-(const Coord &lhs, const Coord &rhs);	 // To find double pushes