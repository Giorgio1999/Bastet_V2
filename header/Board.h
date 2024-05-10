#pragma once
#include "BitBoardUtility.h"
#include "Move.h"
#include <string>
#include <cstdint>

class Board
{
public:
	bitboard pieceBoards[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // Bitboards for all the pieces 0,...5: white, 6,...11: black
																		  //  0/6: pawn, 1/7: knight, 2/8: bishop, 3/9: rook, 4/10: queen, 5/11 king
	bitboard ghostBoard = 0;										  // Board containing enpassant ghosts
	bitboard colorBoards[2] = {0, 0};								  // Board containing all pieces of one color: 0 white, 1 black
	bitboard attackBoard = 0;										  // Bitboard containing all attacks of the player not to move
	bool whiteToMove = true;											  // Color to move
	bool castlingRights[4] = {false, false, false, false};				  // Castling rights: {whiteKing,whiteQueen,blackKing,blackQueen}
	bool enpassantable = false;											  // flag if enpassant is possible
	int kingIndices[2] = {0, 0};										  // quick reference to kingIndices
	bool isCheck = false;

	Board();
	void Clear();
	void MakeMove(const Move &move);		  // makes move and updates board
	void MakeSimpleMove(const Move &move);	  // makes simple move which is expected to not be king move
	void UpdateColorBoards();				  // Updates the color boards
	void InitialiseKingIndices();			  // finds the initial king coords
	void UpdateKingIndices(const Move &move); // Updates the king coords after a move

	std::string ShowBoard();
};
