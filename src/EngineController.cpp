#include "EngineController.h"
#include "Engine.h"
#include "FenParser.h"
#include "BitBoardUtility.h"
#include <string>
#include <list>
#include <chrono>
#include <iostream>
#include <fstream>
#include <array>

EngineController::EngineController() {}

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
	std::vector<Mover> moves = Str2Moves(moveHistory);
	for (const auto &move : moves)
	{
		engine.MakeMove(move);
	}
}

bool EngineController::IsReady()
{
	if (!isReady)
	{
		BootEngine();
	}
	return isReady;
}

std::string EngineController::ShowBoard()
{
	return engine.ShowBoard();
}

std::string EngineController::GetLegalMoves()
{
	std::array<Mover,320> moveHolder;
	uint moveHolderIndex = 0;
	engine.GetLegalMoves(moveHolder,moveHolderIndex);
	std::string movesString = "";
	for (uint i = 0;i<moveHolderIndex;i++)
	{
		movesString += Move2Str(moveHolder[i]) + " ";
	}
	return movesString;
}

std::string EngineController::Search()
{
	return Move2Str(engine.GetBestMove());
}

std::string EngineController::Perft(const int &depth)
{
	// auto start = std::chrono::high_resolution_clock::now();
	int numberOfLeafs = engine.Perft(depth);
	// auto end = std::chrono::high_resolution_clock::now();
	// float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	// auto mnps = numberOfLeafs / duration / 1000000. * 1000.;
	// std::string returnString = "Positions found: " + std::to_string(numberOfLeafs) + ", Speed = " + std::to_string(mnps) + "Mn/s";
	std::string returnString = std::to_string(numberOfLeafs);
	return returnString;
}

std::string EngineController::SplitPerft(const int &depth)
{
	std::array<Mover,320> moveHolder;
	uint moveHolderIndex=0;
	engine.GetLegalMoves(moveHolder,moveHolderIndex);
	std::string returnString = "";
	for (uint i = 0; i < moveHolderIndex; i++)
	{
		returnString += Move2Str(moveHolder[i]) += ": ";
		engine.MakeMove(moveHolder[i]);
		auto res = 0;
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

void EngineController::UndoLastMove()
{
	engine.UndoLastMove();
}

void EngineController::TestReady()
{
	isReady = true;
}

void EngineController::FullPerftTest()
{
	if (!isReady)
	{
		BootEngine();
	}
	auto fullStart = std::chrono::high_resolution_clock::now();
	auto dataPath = "/home/giorgio/Bastet_V2/assets/perftTestSuit.txt";
	std::fstream dataStream(dataPath, std::ios::in);
	std::string line;
	while (std::getline(dataStream, line) && !engine.stopFlag)
	{
		std::string fen = line.substr(0, line.find(','));
		std::cout << "Position: " << fen << ":" << std::endl;
		engine.SetBoard(Fen2Position(fen));
		line = line.substr(fen.size() + 1, line.size());
		auto depth = 1;
		while (line.size() > 0 && !engine.stopFlag)
		{
			std::cout << "\tdepth " << depth << ": ";
			auto start = std::chrono::high_resolution_clock::now();
			auto result = engine.Perft(depth);
			auto end = std::chrono::high_resolution_clock::now();
			float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			auto mnps = result / duration / 1000000. * 1000.;
			std::string ref = line.substr(0, line.find_first_of(';'));
			std::cout << result << "(" << ref << ")";
			auto diff = result - std::stoi(ref);
			std::cout << " diff: " << diff << ", speed: " << mnps << "Mnps" << std::endl;
			line = line.substr(ref.size() + 1, line.size());
			depth++;
		}
	}
	auto fullEnd = std::chrono::high_resolution_clock::now();
	float duration = std::chrono::duration_cast<std::chrono::minutes>(fullEnd - fullStart).count();
	std::cout << "Done! Total time: " << duration << "m" << std::endl;
	dataStream.close();
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
	auto dataPath = "/home/giorgio/Bastet_V2/assets/perftTestSuit.txt";
	std::fstream dataStream(dataPath, std::ios::in);
	std::string line;
	auto valid = true;
	while (std::getline(dataStream, line) && valid)
	{
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
	std::cout << (valid ? "Is valid" : "Is not valid") << std::endl;
	dataStream.close();
}