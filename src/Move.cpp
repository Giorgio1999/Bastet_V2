#include "Move.h"
#include <string>
#include <cmath>
#include <vector>

Coord::Coord()
{
	x = 0;
	y = 0;
}
Coord::Coord(const int &_x, const int &_y)
{
	x = _x;
	y = _y;
}

Move::Move()
{
	startIndex = 0;
	targetIndex = 0;
}

Move::Move(const int &start, const int &target)
{
	startIndex = start;
	targetIndex = target;
}

Move::Move(const Coord &_startCoord, const Coord &_targetCoord)
{
	startCoord = _startCoord;
	targetCoord = _targetCoord;
	startIndex = _startCoord.y*8+_startCoord.x;
	targetIndex = _targetCoord.y*8+_targetCoord.x;
}

Move::Move(const int &i1, const int &j1, const int &i2, const int &j2)
{
	startCoord = Coord(i1, j1);
	targetCoord = Coord(i2, j2);
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

std::string Move2Str(const Move &move)
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
		tmp += PieceType2Str(move.convertTo);
	}
	return tmp;
}

Move Str2Move(const std::string &moveString)
{
	Move move = Move(Str2Coord(moveString.substr(0, 2)), Str2Coord(moveString.substr(2, 4)));
	if (moveString.length() == 5)
	{
		switch (moveString[moveString.length() - 1])
		{
		case 'q':
			move.convertTo = queen;
			break;
		case 'n':
			move.convertTo = knight;
			break;
		case 'b':
			move.convertTo = bishop;
			break;
		case 'r':
			move.convertTo = rook;
			break;
		}
	}
	return move;
}

std::vector<Move> Str2Moves(std::string &movesString)
{
	std::vector<Move> moves;
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

bool operator!=(const Move &lhs, const Move &rhs)
{
	return lhs.startCoord == rhs.startCoord && lhs.targetCoord == rhs.targetCoord;
}

bool operator==(const Move &lhs, const Move &rhs)
{
	return lhs.startCoord == rhs.startCoord && lhs.targetCoord == rhs.targetCoord;
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