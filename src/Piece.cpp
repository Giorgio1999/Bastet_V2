#include "Piece.h"
#include <string>

Piece::Piece()
{
	color = true;
	pieceType = none;
};

Piece::Piece(const bool &_color, const PieceType &_pieceType)
{
	color = _color;
	pieceType = _pieceType;
}

std::string PieceType2Str(const PieceType &type)
{
	std::string tmp = "";
	tmp += types.at((int)type);
	return tmp;
}