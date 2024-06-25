#pragma once
#include "BitBoardUtility.h"
#include "Board.h"
#include <string>

// This is all the functionality required to translate between "human readable" fen and engine language
// Fen parsing
// -------------------------------------------------------------------
Board Fen2Position();                       // Fen Parser for starting position
Board Fen2Position(const std::string &fen); // Fen Parser
// -------------------------------------------------------------------