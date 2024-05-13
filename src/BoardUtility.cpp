#include "BoardUtility.h"
#include "BitBoardUtility.h"
#include <string>
#include <cmath>
#include <vector>

Coord::Coord()
{
}
Coord::Coord(const int &_x, const int &_y)
{
	x = _x;
	y = _y;
}

Mover::Mover()
{
	startIndex = 0;
	targetIndex = 0;
}

Mover::Mover(const int &start, const int &target)
{
	startIndex = start;
	targetIndex = target;
}

Mover::Mover(const Coord &_startCoord, const Coord &_targetCoord)
{
	startIndex = _startCoord.y*8+_startCoord.x;
	targetIndex = _targetCoord.y*8+_targetCoord.x;
}

Mover::Mover(const int &i1, const int &j1, const int &i2, const int &j2)
{
	startIndex = j1 * 8 + i1;
	targetIndex = j2 * 8 + i2;
}

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
		tmp += types.at(move.convertTo+1);
	}
	return tmp;
}

Mover Str2Move(const std::string &moveString)
{
	Mover move = Mover(Str2Coord(moveString.substr(0, 2)), Str2Coord(moveString.substr(2, 4)));
	if (moveString.length() == 5)
	{
		switch (moveString[moveString.length() - 1])
		{
		case 'q':
			move.convertTo = 4;
			break;
		case 'n':
			move.convertTo = 1;
			break;
		case 'b':
			move.convertTo = 2;
			break;
		case 'r':
			move.convertTo = 3;
			break;
		}
	}
	return move;
}

std::vector<Mover> Str2Moves(std::string &movesString)
{
	std::vector<Mover> moves;
	std::string currentMove = "";
	while (movesString.length() > 0)
	{
		std::string currentMove = movesString.substr(0, movesString.find(' '));
		moves.push_back(Str2Move(currentMove));
		if (movesString.length() == currentMove.length())
		{
			break;
		}
		movesString = movesString.substr(currentMove.length() + 1, movesString.length());
	}
	return moves;
}

int Coord2Index(const Coord &coord)
{
	return coord.y * 8 + coord.x;
}

bool operator==(const Coord &lhs, const Coord &rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(const Coord &lhs, const Coord &rhs)
{

	return lhs.x != rhs.x || lhs.y != rhs.y;
}

int operator-(const Coord &lhs, const Coord &rhs)
{
	return lhs.y - rhs.y;
}