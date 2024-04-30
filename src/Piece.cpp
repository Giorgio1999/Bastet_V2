#include "Piece.h"
#include <string>

Piece::Piece() {
	color = true;
	pieceType = none;
};

Piece::Piece(bool _color, PieceType _pieceType) {
	color = _color;
	pieceType = _pieceType;
}

std::string PieceType2Str(PieceType type) {
	std::string tmp = "";
	tmp += types.at((int)type);
	return tmp;
}