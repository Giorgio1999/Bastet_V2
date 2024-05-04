#pragma once
#include "Move.h"
#include <string>
#include <cstdint>

class Board
{
public:
	uint_fast64_t pieceBoards[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Bitboards for all the pieces 0,...5: white, 6,...11: black
																		  //  0/6: pawn, 1/7: knight, 2/8: bishop, 3/9: rook, 4/10: queen, 5/11 king
	uint_fast64_t ghostBoard = 0;										  // Board containing enpassant ghosts
	uint_fast64_t colorBoards[2] = {0, 0};								  // Board containing all pieces of one color: 0 white, 1 black
	bool whiteToMove = true;											  // Color to move
	bool castlingRights[4] = {false, false, false, false};				  // Castling rights: {whiteKing,whiteQueen,blackKing,blackQueen}
	bool enpassantable = false;											  // flag if enpassant is possible
	Coord kingCoords[2] = {Coord(), Coord()};							  // quick reference for king coords

	Board();
	void Clear();
	void MakeMove(const Move &move);		 // makes move and updates board
	void UpdateColorBoards();				 // Updates the color boards
	void InitialiseKingCoords();			 // finds the initial king coords
	void UpdateKingCoords(const Move &move); // Updates the king coords after a move

	std::string ShowBoard();
};
