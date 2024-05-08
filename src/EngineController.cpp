#include "EngineController.h"
#include "Engine.h"
#include "FenParser.h"
#include <string>
#include <list>
#include <chrono>
#include <iostream>
#include <fstream>

EngineController::EngineController() {}

void EngineController::BootEngine()
{
	engine = Engine();
	startpos = Fen2Position();
	isReady = true;
}

void EngineController::SetStopFlag(const bool& value){
	engine.stopFlag = value;
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

std::string EngineController::Search()
{
	return Move2Str(engine.GetBestMove());
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

void EngineController::FullPerftTest(){
	auto dataPath = "C:/Users/giorg/Documents/Projekte/ChessEngine/Bastet_V2/assets/perftTestSuit.txt";
	std::fstream dataStream(dataPath,std::ios::in);
	std::string line;
	// while(std::getline(dataStream,line) && !engine.stopFlag){
	// 	std::string fen = line.substr(0,line.find(','));
	// 	std::cout << "Position: " << fen << ":\n";
	// 	engine.SetBoard(Fen2Position(fen));
	// 	line = line.substr(fen.size()+1,line.size());
	// 	auto depth =1;
	// 	while(line.size()>0 && !engine.stopFlag){
	// 		std::cout << "\tdepth " << depth << ": "; 
	// 		auto result = engine.Perft(depth);
	// 		std::string ref = line.substr(0,line.find_first_of(';'));
	// 		std::cout << result << "(" << ref << ")";
	// 		auto diff = result-std::stoi(ref);
	// 		std::cout << " diff: " << diff << "\n";
	// 		line = line.substr(ref.size()+1,line.size());
	// 		depth++;
	// 	}
	// }
	dataStream.close();
}