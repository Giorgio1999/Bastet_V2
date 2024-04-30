#include "Engine.h"
#include "Board.h"
#include "Piece.h"
#include "Move.h"
#include <string>
#include <vector>

Engine::Engine(){
	board = Board();
}

void Engine::NewGame(){
	board.Clear();
}

void Engine::SetBoard(Board newBoard) {
	board = newBoard;
}

void Engine::MakeMove(Move move) {
	gameHistory.push_back(board);
	board.MakeMove(move);
}

void Engine::UndoLastMove() {
	board = gameHistory.back();
	gameHistory.pop_back();
}

std::string Engine::ShowBoard() {
	return board.ShowBoard();
}

void Engine::GetPseudoLegalMoves(std::vector<Move>& moveVector) {
}

void Engine::GetPseudoLegalPawnMoves(std::vector<Move>& moveVector) {
}

void Engine::GetPseudoLegalKnightMoves(std::vector<Move>& moveVector) {
}

void Engine::GetPseudoLegalBishopMoves(std::vector<Move>& moveVector){
}

void Engine::GetPseudoLegalRookMoves(std::vector<Move>& moveVector){
}

void Engine::GetPseudoLegalQueenMoves(std::vector<Move>& moveVector){
	GetPseudoLegalRookMoves(moveVector);
	GetPseudoLegalBishopMoves(moveVector);
}

void Engine::GetPseudoLegalKingMoves(std::vector<Move>& moveVector){
}

Move Engine::GetBestMove() {
	std::vector<Move> legalmoves;
	GetLegalMoves(legalmoves);
	return legalmoves[0];
}

void Engine::GetLegalMoves(std::vector<Move>& moveVector) {
	std::vector<Move> pseudoMoves;
	GetPseudoLegalMoves(pseudoMoves);
}

int Engine::Perft(int depth) {
	std::vector<Move> legalMoves;
	GetLegalMoves(legalMoves);
	if (depth <= 0) {
		return (int)legalMoves.size();
	}
	int numberOfLeafs = 0;
	for (const auto& current : legalMoves) {
		board.MakeMove(current);
		numberOfLeafs += Perft(depth-1);
	}
	return numberOfLeafs;
}