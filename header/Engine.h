#pragma once
#include "Board.h"
#include "Piece.h"
#include "Move.h"
#include <string>
#include <vector>

// This will include the core functionality

class Engine
{
public:
	std::vector<Board> gameHistory; // History of the game, consisting of boards. For undo or perhaps draw checks?
	Board board;					// Board representation
	Engine();
	void NewGame();						   // clears board for new game
	void SetBoard(const Board &board);	   // Sets a given PieceList as position
	void MakeMove(const Move &move);	   // Make a move
	void MakeSimpleMove(const Move &move); // Makes a simple move (expected move is non king move)
	void UndoLastMove();				   // Reverts to previous position
	std::string ShowBoard();			   // Debug tool to display current board in console

	void GetPseudoLegalMoves(std::vector<Move> &pseudoLegalMoves); // Gets pseudolegal moves. Implementation in MoveGenerator.cpp
	void GetLegalMoves(std::vector<Move> &legalMoves);			   // Gets legalMoves

	void GenerateAttacks(const bool &color);

	Move GetBestMove(); // Returns best move after search

	int Perft(const int &depth);
};