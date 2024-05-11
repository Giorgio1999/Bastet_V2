#pragma once
#include "BitBoardUtility.h"
#include "Engine.h"
#include "Board.h"
#include <string>


// This will serve as an mediator between the uci handler and the engine
class EngineController
{
	const char* benchMarkingData[6] = {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1","r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
											"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1","r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
											"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8","r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"};
	Engine engine;		  // holds instance of engine
	bool isReady = false; // no use as of yet
	Board startpos;

public:
	EngineController();
	bool BootEngine();								// Boot engine
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
	void FullPerftTest();					//Runs a full perft test suite
	void Bench();
};