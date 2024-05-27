#pragma once
#include "BitBoardUtility.h"
#include "Board.h"
#include "BoardUtility.h"
#include "Timer.h"
#include <string>
#include <array>
#include <vector>

// This will include the core functionality
// Engine class
// --------------------------------------------------------------
class Engine
{
public:
	int maxDepth = 4;					   // Maximal depth
	bool stopFlag;						   // Flag that can be set from the controller to stop the engine
	Board gameHistory[11];				   // History of the game, consisting of boards.
	uint gameHistoryIndex = 0;			   // Index to the current board
	bitboard hashes[12 * 64 + 1 + 4 + 8];  // Hash table for: pieces*square + black to move + castling rights + enpassant file
	std::vector<bitboard> repetitionTable; // Table of zobrist hashes to check for repetitions
	bitboard currentZobristKey;			   // Zobrist key of the current position

	// Initialising and accessing the engine
	// --------------------------------------------------------------------------------------------
	Engine();
	void Boot();
	void NewGame();					   // clears board for new game
	void SetBoard(const Board &board); // Resets engine and sets board as current board
	Board &CurrentBoard();			   // Reference to the current board
	// --------------------------------------------------------------------------------------------

	// Core functionality
	// --------------------------------------------------------------------------------------------
	void MakeMove(const move &move);											  // Make a move
	void MakeSimpleMove(const move &move);										  // Make a simple move
	void MakePermanentMove(const move &move);									  // Make a move that can not be undone
	void UndoLastMove();														  // Revert to previous position
	void GetLegalMoves(std::array<move, 256> &moveHolder, uint &moveHolderIndex); // Gets legalMoves
	// --------------------------------------------------------------------------------------------

	// Search
	// --------------------------------------------------------------------------------------------
	Mover GetBestMove(const Timer &timer); // Searches for the best move
	// --------------------------------------------------------------------------------------------

	// Performance evaluation
	// --------------------------------------------------------------------------------------------
	bitboard Perft(int depth); // Counts all leafs up to fixed depth of the current board
	// --------------------------------------------------------------------------------------------

	// Debugging
	// --------------------------------------------------------------------------------------------
	std::string ShowBoard(); // Debug tool to display current board in console
							 // --------------------------------------------------------------------------------------------
};
// ------------------------------------------------------------