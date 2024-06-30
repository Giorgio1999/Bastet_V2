#pragma once
#include "BitBoardUtility.h"
#include "Board.h"
#include "BoardUtility.h"
#include "Timer.h"
#include "MathUtility.h"
#include "TranspositionTable.h"
#include <string>
#include <array>
#include <vector>

// This will include the core functionality
// Engine class
// --------------------------------------------------------------
class Engine
{
public:
	int maxDepth = 200;						  // Maximal depth
	bool stopFlag;							  // Flag that can be set from the controller to stop the engine
	Board gameHistory[200];					  // History of the game, consisting of boards.
	uint gameHistoryIndex = 0;				  // Index to the current board
	std::vector<bitboard> repetitionTable;	  // Table of zobrist hashes to check for repetitions
	bitboard currentZobristKey;				  // Zobrist key of the current position
	MathUtility::Random<int> *prng = nullptr; // PseudoNumberGenerator
	bitboard ttSize = 64;					  // Transposition table size in MB
	transposition::Tt tt;					  // Transposition table

	// Initialising and accessing the engine
	// --------------------------------------------------------------------------------------------
	Engine();
	void Boot();
	void NewGame();					   // clears board for new game
	void SetBoard(const Board &board); // Resets engine and sets board as current board
	bitboard InitialiseZobristHash();  // Initialises the Zobrist Key
	Board &CurrentBoard();			   // Reference to the current board
	void SetTtSize(int _ttSize);	   // Resizes tt table
	// --------------------------------------------------------------------------------------------

	// Core functionality
	// --------------------------------------------------------------------------------------------
	void MakeMove(const move &move);																 // Make a move
	void MakeSimpleMove(const move &move);															 // Make a simple move
	void MakePermanentMove(const move &move);														 // Make a move that can not be undone
	void UndoLastMove();																			 // Revert to previous position
	void UndoLastSimpleMove();																		 // Undo a simple move
	void GetLegalMoves(std::array<move, 256> &moveHolder, uint &moveHolderIndex, bool capturesOnly); // Gets legalMoves, if capturesOnly get legal captures
	bool IsCheck();																					 // Return wheter player to move is in check
	// --------------------------------------------------------------------------------------------

	// Search
	// --------------------------------------------------------------------------------------------
	Mover GetBestMove(Timer &timer); // Searches for the best move
	// --------------------------------------------------------------------------------------------

	// Performance evaluation
	// --------------------------------------------------------------------------------------------
	bitboard Perft(int depth); // Counts all leafs up to fixed depth of the current board
	// --------------------------------------------------------------------------------------------

	// Debugging
	// --------------------------------------------------------------------------------------------
	std::string ShowBoard();  // Debug tool to display current board in console
	float Evaluate();		  // Debug tool to return static evaluation of current position
	void HashTest(int depth); // Testing Zobrist Key generation
							  // --------------------------------------------------------------------------------------------
};
// ------------------------------------------------------------