#pragma once
#include <string>

// This is the piece struct. Each piece has a type and nothing else so far

enum PieceType
{
	none,
	pawn,
	knight,
	bishop,
	rook,
	queen,
	king
}; // Piece types including none for empty squares
const std::string types = "-pnbrqk";

struct Piece
{
	bool color; // Color of piece
	PieceType pieceType;

	Piece();
	Piece(const bool &_color, const PieceType &_pieceType); // simple constructor
};

std::string PieceType2Str(const PieceType &type);
