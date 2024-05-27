#include "BoardUtility.h"
#include "BitBoardUtility.h"
#include <string>
#include <vector>
#include <regex>

// Datastruct for Squares
// -----------------------------------------------------------
Coord::Coord()
{
}
Coord::Coord(const int &_x, const int &_y)
{
	x = _x;
	y = _y;
}
// -----------------------------------------------------------

// Different specialised constructors
// -----------------------------------------------------------
Mover::Mover() {}

Mover::Mover(const int &start, const int &target)
{
	startIndex = start;
	targetIndex = target;
}

Mover::Mover(const Coord &_startCoord, const Coord &_targetCoord)
{
	startIndex = _startCoord.y * 8 + _startCoord.x;
	targetIndex = _targetCoord.y * 8 + _targetCoord.x;
}

Mover::Mover(const int &i1, const int &j1, const int &i2, const int &j2)
{
	startIndex = j1 * 8 + i1;
	targetIndex = j2 * 8 + i2;
}
// -----------------------------------------------------------

// Conversion functions
// -----------------------------------------------------------
std::string Coord2Str(const Coord &coord)
{
	std::string tmp = "";
	tmp += cols.at(coord.x);
	tmp += rows.at(coord.y);
	return tmp;
}

Coord Str2Coord(const std::string &coordString)
{
	return Coord(cols.find(coordString[0]), rows.find(coordString[1]));
}

std::string Move2Str(const Mover &move)
{
	std::string tmp = "";
	auto i = move.startIndex % 8;
	auto j = (move.startIndex - i) / 8;
	tmp += cols.at(i);
	tmp += rows.at(j);
	i = move.targetIndex % 8;
	j = (move.targetIndex - i) / 8;
	tmp += cols.at(i);
	tmp += rows.at(j);
	if (move.promotion)
	{
		tmp += types.at(move.convertTo + 1);
	}
	return tmp;
}

Mover Str2Move(const std::string &moveString)
{
	Mover move = Mover(Str2Coord(moveString.substr(0, 2)), Str2Coord(moveString.substr(2, 2)));
	if (moveString.length() == 5)
	{
		move.promotion = true;
		switch (moveString[moveString.length() - 1])
		{
		case 'n':
			move.convertTo = 1;
			break;
		case 'b':
			move.convertTo = 2;
			break;
		case 'r':
			move.convertTo = 3;
			break;
		case 'q':
			move.convertTo = 4;
			break;
		}
	}
	return move;
}

std::vector<Mover> Str2Moves(std::string &movesString)
{
	std::vector<Mover> moves;
	// Courtesy of chatgbt
	std::regex move_regex("\\S+");																// create regex for chain of non space characters
	auto move_begin = std::sregex_iterator(movesString.begin(), movesString.end(), move_regex); // Get iterator of front
	auto move_end = std::sregex_iterator();														// Get Iterator of back
	for (std::sregex_iterator i = move_begin; i != move_end; i++)
	{							// Loop over all matches
		std::smatch match = *i; // ?
		moves.push_back(Str2Move(match.str()));
	}
	return moves;
}

Mover Move2Mover(const move &move)
{
	Mover mover = Mover(StartIndex(move), TargetIndex(move));
	mover.convertTo = ConvertTo(move);
	mover.promotion = (bool)Promotion(move);
	return mover;
}

move Mover2Move(const Mover &mover)
{
	return Move(mover.startIndex, mover.targetIndex, mover.convertTo, mover.promotion);
}
// -----------------------------------------------------------