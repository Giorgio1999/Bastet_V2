#pragma once
#include "BitBoardUtility.h"
#include "Board.h"
#include "BoardUtility.h"
#include "Timer.h"
#include <string>
#include <vector>
#include <atomic>
#include <array>

// This will include the core functionality

class Engine
{
public:
	int maxDepth = 4;
	bool stopFlag;
	Board gameHistory[20];	   // History of the game, consisting of boards. For undo or perhaps draw checks?
	uint gameHistoryIndex = 0; // Index to the end of the gameHistory
	Engine();
	void Boot();
	void NewGame();						   // clears board for new game
	void SetBoard(const Board &board);	   // Sets a given PieceList as position
	void MakeMove(const move &move);	   // Make a move
	void MakeSimpleMove(const move &move); // Makes a simple move (expected move is non king move)
	void UndoLastMove();				   // Reverts to previous position
	std::string ShowBoard();			   // Debug tool to display current board in console

	void GetPseudoLegalMoves(std::array<move, 256> &moveHolder, uint &moveHolderIndex); // Gets pseudolegal moves. Implementation in MoveGenerator.cpp
	void GetLegalMoves(std::array<move, 256> &moveHolder, uint &moveHolderIndex);		// Gets legalMoves

	Mover GetBestMove(const Timer &timer); // Returns best move after search

	int Perft(const int &depth);
};