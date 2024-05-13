#pragma once
#include "BitBoardUtility.h"
#include <string>
#include <list>
#include <vector>

// Logic for handling moves

const std::string rows = "87654321"; // Hardcoded translations of the rows or y
const std::string cols = "abcdefgh"; // Hardcoded translations of the cols or x
const std::string types = "-pnbrqk";	// Hardcoded translations from int to piecetypes

struct Coord
{
	int x;
	int y;

	Coord();
	Coord(const int &_x, const int &_y);
};

class Mover
{
	public:
		int startIndex;	//From
		int targetIndex;	//To

		// PieceType convertTo = none; // used to indicate promotions, as well as using wheter the move is a king move or not for checks
		int convertTo = 0;	// used to indicate Promotions: 0=none,1=knight,2=bishop,3=rook,4=queen
		bool promotion = false;		// distinguish promotions from none promotions

		Mover();
		Mover(const int& start,const int&target);
		Mover(const int &i1, const int &j1, const int &i2, const int &j2);
		Mover(const Coord &_startCoord, const Coord &_targetCoord);
};

std::string Coord2Str(const Coord &coord); // Helper functions to convert from human readable strings to moves and coords
Coord Str2Coord(const std::string &coordString);
Mover Str2Move(const std::string &moveString);
std::string Move2Str(const Mover &move);
std::vector<Mover> Str2Moves(std::string &movesString);
Mover Move2Mover(const move& move);
move Mover2Move(const Mover& mover);
int Coord2Index(const Coord &coord);			   // convert between coord and 1d Board index
bool operator==(const Coord &lhs, const Coord &rhs); // To compare coords
bool operator!=(const Coord &lhs, const Coord &rhs); // negation
int operator-(const Coord &lhs, const Coord &rhs);	 // To find double pushes