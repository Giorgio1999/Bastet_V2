#pragma once
#include "BitBoardUtility.h"
#include "Board.h"
#include <string>

// This is all the functionality required to translate between "human readable" fen and engine language

// const std::string startingPosition = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"; // hardcoded starting position
// const std::string startingPosition = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"; //temporary tsting
const std::string startingPosition = "r7/7k/8/8/8/8/8/K7 w - - 0 1";
const std::string whitePieceCodings = "pnbrqk";
const std::string blackPieceCodings = "PNBRQK";

Board Fen2Position();                       // Fen Parser for starting position
Board Fen2Position(const std::string &fen); // FFen Parser