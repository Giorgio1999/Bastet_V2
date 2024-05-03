#include "EngineController.h"
#include "Engine.h"
#include "FenParser.h"
#include <string>
#include <list>
#include <chrono>

EngineController::EngineController(){}

void EngineController::BootEngine() {
	engine = Engine();
	startpos = Fen2Position();
	isReady = true;
}

void EngineController::NewGame() {
	engine.NewGame();
}

void EngineController::SetPosition() {
	if(!isReady){
		startpos = Fen2Position();
	}
	engine.SetBoard(startpos);
}

void EngineController::SetPosition(std::string fenString) {
	engine.SetBoard(Fen2Position(fenString));
}

void EngineController::MakeMoves(std::string moveHistory) {
	std::vector<Move> moves = Str2Moves(moveHistory);
	for (const auto& move : moves) {
		engine.MakeMove(move);
	}
}

bool EngineController::IsReady() {
	return isReady;
}

std::string EngineController::ShowBoard() {
	return engine.ShowBoard();
}

std::string EngineController::GetLegalMoves() {
	return "";
}

std::string EngineController::Search() {
	return Move2Str(engine.GetBestMove());
}

std::string EngineController::Perft(int depth) {
	auto start = std::chrono::high_resolution_clock::now();
	int numberOfLeafs = engine.Perft(depth);
	auto end = std::chrono::high_resolution_clock::now();
	float duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	auto mnps = numberOfLeafs / duration / 1000000. * 60.;
	std::string returnString = "Positions found: " + std::to_string(numberOfLeafs) + ", Speed = " + std::to_string(mnps) + "Mn/s";
	return returnString;
}

std::string EngineController::SplitPerft(int depth) {
	//TO DO: Split perft implementation
	return "";
}

void EngineController::UndoLastMove() {
	engine.UndoLastMove();
}

void EngineController::TestReady(){
	isReady = true;
}