#pragma once
#include "BitBoardUtility.h"
#include "Board.h"
#include "Move.h"
#include <string>
#include <vector>
#include <atomic>

// This will include the core functionality

class Engine
{
public:
	bool stopFlag;
	std::vector<Board> gameHistory; // History of the game, consisting of boards. For undo or perhaps draw checks?
	Board board;					// Board representation
	Engine();
	void Boot();
	void NewGame();						   // clears board for new game
	void SetBoard(const Board &board);	   // Sets a given PieceList as position
	void MakeMove(const Move &move);	   // Make a move
	void CopyMake(const Move &move);	   // Copy board then make move on it
	void MakeSimpleMove(const Move &move); // Makes a simple move (expected move is non king move)
	void UndoLastMove();				   // Reverts to previous position
	std::string ShowBoard();			   // Debug tool to display current board in console

	void GetPseudoLegalMoves(std::vector<Move> &pseudoLegalMoves); // Gets pseudolegal moves. Implementation in MoveGenerator.cpp
	void GetLegalMoves(std::vector<Move> &legalMoves);			   // Gets legalMoves

	Move GetBestMove(); // Returns best move after search

	int Perft(const int &depth);
};