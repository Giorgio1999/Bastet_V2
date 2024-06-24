#include "EngineController.h"
#include "Engine.h"
#include "FenParser.h"
#include "BitBoardUtility.h"
#include "Timer.h"
#include "Data.h"
#include <string>
#include <chrono>
#include <iostream>
#include <array>

EngineController::EngineController() {}

// UCI
// -------------------------------------------------------------------
bool EngineController::BootEngine()
{
	engine = Engine();
	startpos = Fen2Position();
	engine.Boot();
	isReady = true;
	return true;
}

void EngineController::SetStopFlag(const bool &value)
{
	engine.stopFlag = value;
}

void EngineController::NewGame()
{
	if (!isReady)
	{
		BootEngine();
	}
	engine.NewGame();
}

void EngineController::SetPosition()
{
	if (!isReady)
	{
		BootEngine();
	}
	engine.SetBoard(startpos);
}

void EngineController::SetPosition(const std::string &fenString)
{

	if (!isReady)
	{
		BootEngine();
	}
	engine.SetBoard(Fen2Position(fenString));
}

void EngineController::MakeMoves(std::string &moveHistory)
{
	std::vector<Mover> movers = Str2Moves(moveHistory);
	for (const auto &mover : movers)
	{
		engine.MakePermanentMove(Mover2Move(mover));
	}
}

void EngineController::TestReady()
{
	isReady = true;
}

bool EngineController::IsReady()
{
	if (!isReady)
	{
		BootEngine();
	}
	return isReady;
}

std::string EngineController::Search(const int wTime, const int bTime, const int winc, const int binc)
{
	Timer timer(wTime, bTime, winc, binc);
	return Move2Str(engine.GetBestMove(timer));
}

std::string EngineController::Options()
{
	std::string res = "option name Hash type spin default " + std::to_string(engine.ttSize) + " min 0 max 128";
	return res;
}

void EngineController::SetOptions(int _ttSize)
{
	engine.ttSize = _ttSize;
	BootEngine();
}
// -------------------------------------------------------------------

// Non UCI
// -------------------------------------------------------------------
std::string EngineController::Perft(int depth)
{
	bitboard numberOfLeafs = engine.Perft(depth);
	std::string returnString = std::to_string(numberOfLeafs);
	return returnString;
}

std::string EngineController::SplitPerft(int depth)
{
	std::array<move, 256> moveHolder;
	uint moveHolderIndex = 0;
	engine.GetLegalMoves(moveHolder, moveHolderIndex, false);
	std::string returnString = "";
	for (uint i = 0; i < moveHolderIndex; i++)
	{
		returnString += Move2Str(Move2Mover(moveHolder[i])) += ": ";
		engine.MakeMove(moveHolder[i]);
		bitboard res = 0;
		if (depth != 1)
		{
			res = engine.Perft(depth - 1);
		}
		else
		{
			res = 1;
		}
		returnString += std::to_string(res);
		engine.UndoLastMove();
		returnString += "\n";
	}
	return returnString;
}

void EngineController::FullPerftTest()
{
	if (!isReady)
	{
		BootEngine();
	}
	auto fullStart = std::chrono::high_resolution_clock::now();
	for (uint i = 0; i < fullPerftSuite.size(); i++)
	{
		std::string line = fullPerftSuite.at(i);
		std::string fen = line.substr(0, line.find(','));
		std::cout << "Position: " << fen << ":" << std::endl;
		engine.SetBoard(Fen2Position(fen));
		line = line.substr(fen.size() + 1, line.size());
		auto depth = 1;
		while (line.size() > 0 && !engine.stopFlag)
		{
			std::cout << "\tdepth " << depth << ": ";
			auto start = std::chrono::high_resolution_clock::now();
			bitboard result = engine.Perft(depth);
			auto end = std::chrono::high_resolution_clock::now();
			float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			auto mnps = result / duration / 1000000. * 1000.;
			std::string ref = line.substr(0, line.find_first_of(';'));
			std::cout << result << "(" << ref << ")";
			int diff = result - std::stoi(ref);
			std::cout << " diff: " << diff << ", speed: " << mnps << "Mnps" << std::endl;
			line = line.substr(ref.size() + 1, line.size());
			depth++;
		}
	}
	auto fullEnd = std::chrono::high_resolution_clock::now();
	float duration = std::chrono::duration_cast<std::chrono::seconds>(fullEnd - fullStart).count();
	std::cout << "Done! Total time: " << duration << "s" << std::endl;
}

void EngineController::Bench()
{
	if (!isReady)
	{
		BootEngine();
	}
	auto fixedDepth = 4;
	auto fullStart = std::chrono::steady_clock::now();
	bitboard nodesVisited = 0;
	for (uint i = 0; i < benchMarkingData.size(); i++)
	{
		std::string fen = benchMarkingData.at(i);
		engine.SetBoard(Fen2Position(fen));
		nodesVisited += engine.Perft(fixedDepth);
	}
	auto fullEnd = std::chrono::steady_clock::now();
	float duration = std::chrono::duration_cast<std::chrono::milliseconds>(fullEnd - fullStart).count();
	std::cout << std::to_string(nodesVisited) << " nodes " << std::to_string((int)(nodesVisited / duration * 1000)) << " nps" << std::endl;
}

void EngineController::Validate()
{
	if (!isReady)
	{
		BootEngine();
	}
	auto start = std::chrono::steady_clock::now();
	auto valid = true;
	for (uint i = 0; i < fullPerftSuite.size(); i++)
	{
		std::string line = fullPerftSuite.at(i);
		std::string fen = line.substr(0, line.find(','));
		engine.SetBoard(Fen2Position(fen));
		line = line.substr(fen.size() + 1, line.size());
		auto depth = 1;
		while (line.size() > 0 && !engine.stopFlag)
		{
			auto result = engine.Perft(depth);
			std::string ref = line.substr(0, line.find_first_of(';'));
			auto diff = result - std::stoi(ref);
			line = line.substr(ref.size() + 1, line.size());
			if (diff != 0)
			{
				valid = false;
				break;
			}
			depth++;
		}
	}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000;
	std::cout << (valid ? "Is valid, " : "Is not valid, ") << std::to_string(duration) << "s" << std::endl;
}

std::string EngineController::Evaluate()
{
	if (!isReady)
	{
		BootEngine();
	}
	std::string result = std::to_string(engine.Evaluate());
	return result;
}
// -------------------------------------------------------------------

// Debugging
// -------------------------------------------------------------------
std::string EngineController::GetLegalMoves(bool capturesOnly)
{
	std::array<move, 256> moveHolder;
	uint moveHolderIndex = 0;
	engine.GetLegalMoves(moveHolder, moveHolderIndex, capturesOnly);
	std::string movesString = "";
	for (uint i = 0; i < moveHolderIndex; i++)
	{
		movesString += Move2Str(Move2Mover(moveHolder[i])) + " ";
	}
	return movesString;
}

std::string EngineController::ShowBoard()
{
	return engine.ShowBoard();
}

void EngineController::UndoLastMove()
{
	engine.UndoLastMove();
}
// -------------------------------------------------------------------
