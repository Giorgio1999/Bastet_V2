#include "Move.h"
#include <string>
#include <cmath>
#include <vector>

Coord::Coord(){
	x = 0;
	y = 0;
}
Coord::Coord(int _x, int _y) {
	x = _x;
	y = _y;
}

Move::Move() {
	startCoord = Coord();
	targetCoord = Coord();
}

Move::Move(Coord _startCoord, Coord _targetCoord) {
	startCoord = _startCoord;
	targetCoord = _targetCoord;
}

Move::Move(int i1, int j1, int i2, int j2) {
	startCoord = Coord(i1, j1);
	targetCoord = Coord(i2, j2);
}

std::string Coord2Str(Coord coord) {
	auto tmp = "";
	tmp += cols.at(coord.x);
	tmp += rows.at(coord.y);
	return tmp;
}

Coord Str2Coord(std::string coordString) {
	return Coord(cols.find(coordString[0]), rows.find(coordString[1]));
}

std::string Move2Str(Move move) {
	std::string tmp = "";
	tmp += Coord2Str(move.startCoord);
	tmp += Coord2Str(move.targetCoord);
	if (move.promotion) {
		tmp += PieceType2Str(move.convertTo);
	}
	return tmp;
}

Move Str2Move(std::string moveString) {
	Move move = Move(Str2Coord(moveString.substr(0, 2)), Str2Coord(moveString.substr(2, 4)));
	if (moveString.length() == 5) {
		switch (moveString[moveString.length() - 1]) {
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

std::vector<Move> Str2Moves(std::string movesString) {
	std::vector<Move> moves;
	std::string currentMove = "";
	while (movesString.length() > 0) {
		std::string currentMove = movesString.substr(0, movesString.find(' '));
		moves.push_back(Str2Move(currentMove));
		if (movesString.length() == currentMove.length()) {
			break;
		}
		movesString = movesString.substr(currentMove.length()+1, movesString.length());
	}
	return moves;
}

int Coord2Index(Coord coord) {
	return coord.y * 8 + coord.x;
}


bool operator!=(Move lhs, Move rhs) {
	return lhs.startCoord == rhs.startCoord && lhs.targetCoord == rhs.targetCoord;
}

bool operator==(Move lhs, Move rhs) {
	return lhs.startCoord == rhs.startCoord && lhs.targetCoord == rhs.targetCoord;
}

bool operator==(Coord lhs, Coord rhs) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

bool operator!=(Coord lhs, Coord rhs) {
	
	return lhs.x != rhs.x || lhs.y != rhs.y;
}

int operator-(Coord lhs, Coord rhs) {
	return lhs.y - rhs.y;
}