#pragma once
#include "BitBoardUtility.h"
#include <string>
#include <cstdint>

class Board
{
public:
	// Board representation holders
	// --------------------------------------------
	bitboard pieceBoards[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Bitboards for all the pieces 0,...5: white, 6,...11: black
																	 //  0/6: pawn, 1/7: knight, 2/8: bishop, 3/9: rook, 4/10: queen, 5/11 king
	bitboard ghostBoard = 0;										 // Board containing enpassant ghosts
	bitboard colorBoards[2] = {0, 0};								 // Board containing all pieces of one color: 0 white, 1 black
	bitboard attackBoard = 0;										 // Bitboard containing all attacks of the player not to move
	flag flags = 0;													 // keeps track of castling rights and player to move: 000qkQKm, potetial space for other stuff
	int kingIndices[2] = {0, 0};									 // quick reference to kingIndices
	// --------------------------------------------

	// Updating the board
	// --------------------------------------------
	Board();											   // Default constructor
	void Clear();										   // Reset board to empty state
	void MakeMove(const move &move, bitboard &zobristKey); // makes move and updates board
	void MakeSimpleMove(const move &move);				   // makes simple move which is expected to not be king move
	void InitialiseColorBoards();						   // Computes colorboards from initial position
	void InitialiseKingIndices();						   // finds the initial king coords
	// --------------------------------------------

	// Visualization tool
	// --------------------------------------------
	std::string ShowBoard(); // Returns a string visual of the current board
							 // --------------------------------------------
};
