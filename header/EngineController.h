#pragma once
#include "BitBoardUtility.h"
#include "Engine.h"
#include "Board.h"
#include <string>
#include <array>
#include <vector>

// This will serve as an mediator between the uci handler and the engine
// Controller class for engines
// -------------------------------------------------------------------
class EngineController
{
	Engine engine;		  // holds instance of engine
	bool isReady = false; // no use as of yet
	Board startpos;		  // Precomputed starting position board

public:
	EngineController();
	// UCI
	// -------------------------------------------------------------------
	bool BootEngine();									  // Boot engine
	void SetStopFlag(const bool &value);				  // Change stop flag to interrupt engine
	void NewGame();										  // Clear board for a new game
	void SetPosition();									  // Set starting position
	void SetPosition(const std::string &fenString);		  // Set position to fen string
	void MakeMoves(std::string &moveHistory);			  // Make given moves on the board
	bool IsReady();										  // return ready flag
	void TestReady();									  // Test wheter engine is ready
	std::string Search(const int wTime, const int bTime); // Search routine
	// -------------------------------------------------------------------

	// Non UCI
	// -------------------------------------------------------------------
	std::string Perft(const int &depth);				  // Debug Routine for testing move generation
	std::string SplitPerft(const int &depth);			  // Split perft prints out number of leaves after each move
	void FullPerftTest();								  // Runs a full perft test suite
	void Bench();										  // Benchmarking with a fixed depth perft on a large number of positions
	void Validate();									  // Validates move generation on test suit
	// -------------------------------------------------------------------

	// Debugging
	// -------------------------------------------------------------------
	std::string GetLegalMoves();						  // Get legalmoves
	std::string ShowBoard();							  // Debug tool to display board in console
	void UndoLastMove();								  // Debug to test functionality
	// -------------------------------------------------------------------
};
// -------------------------------------------------------------------