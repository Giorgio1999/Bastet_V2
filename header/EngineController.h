#pragma once
#include "Engine.h"
#include "Board.h"
#include <string>

// This will serve as an mediator between the uci handler and the engine
class EngineController
{
	Engine engine;		  // holds instance of engine
	bool isReady = false; // no use as of yet
	Board startpos;

public:
	EngineController();
	void BootEngine();								// Boot engine
	void SetStopFlag(const bool& value);
	void NewGame();									// Clears board for a new game
	void SetPosition();								// Sets starting position
	void SetPosition(const std::string &fenString); // Sets position to fen string
	void MakeMoves(std::string &moveHistory);		// Makes given moves on the board
	std::string GetLegalMoves();					// Gets legalmoves
	bool IsReady();									// returns ready flag
	std::string ShowBoard();						// Debug tool to display board in console
	std::string Search();									// Search routine
	std::string Perft(const int &depth);			// Debug Routine for testing move generation
	std::string SplitPerft(const int &depth);		// Split perft prints out number of leaves after each move
	void UndoLastMove();							// Debug to test functionality
	void TestReady();								// Test wheter engine is ready
};