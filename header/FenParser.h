#pragma once
#include "Piece.h"
#include "Board.h"
#include <string>

// This is all the functionality required to translate between "human readable" fen and engine language

const std::string startingPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // hardcoded starting position
const std::string whitePieceCodings = "pnbrqk";
const std::string blackPieceCodings = "PNBRQK";

Board Fen2Position();                       // Fen Parser for starting position
Board Fen2Position(const std::string &fen); // FFen Parser