#include "EngineController.h"
#include "Engine.h"
#include "FenParser.h"
#include <string>
#include <list>
#include <chrono>

EngineController::EngineController() {}

void EngineController::BootEngine()
{
	engine = Engine();
	startpos = Fen2Position();
	isReady = true;
}

void EngineController::NewGame()
{
	engine.NewGame();
}

void EngineController::SetPosition()
{
	if (!isReady)
	{
		startpos = Fen2Position();
	}
	engine.SetBoard(startpos);
}

void EngineController::SetPosition(const std::string &fenString)
{
	engine.SetBoard(Fen2Position(fenString));
}

void EngineController::MakeMoves(std::string &moveHistory)
{
	std::vector<Move> moves = Str2Moves(moveHistory);
	for (const auto &move : moves)
	{
		engine.MakeMove(move);
	}
}

bool EngineController::IsReady()
{
	return isReady;
}

std::string EngineController::ShowBoard()
{
	return engine.ShowBoard();
}

std::string EngineController::GetLegalMoves()
{
	std::vector<Move> legalMoves;
	engine.GetLegalMoves(legalMoves);
	std::string movesString = "";
	for (const auto &move : legalMoves)
	{
		movesString += Move2Str(move) + " ";
	}
	return movesString;
}

void EngineController::Search(std::string&& bestmove)
{
	bestmove =  Move2Str(engine.GetBestMove());
}

std::string EngineController::Perft(const int &depth)
{
	auto start = std::chrono::high_resolution_clock::now();
	int numberOfLeafs = engine.Perft(depth);
	auto end = std::chrono::high_resolution_clock::now();
	float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	auto mnps = numberOfLeafs / duration / 1000000. * 1000.;
	std::string returnString = "Positions found: " + std::to_string(numberOfLeafs) + ", Speed = " + std::to_string(mnps) + "Mn/s";
	return returnString;
}

std::string EngineController::SplitPerft(const int &depth)
{
	std::vector<Move> legalMoves;
	engine.GetLegalMoves(legalMoves);
	std::string returnString = "";
	for (const auto &move : legalMoves)
	{
		engine.MakeMove(move);
		returnString += Move2Str(move) += ": ";
		auto res = 0;
		if(depth != 1){
			res = engine.Perft(depth-2);
		}
		else{
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