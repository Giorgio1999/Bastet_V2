#include "Engine.h"
#include "Board.h"
#include "Piece.h"
#include "Move.h"
#include "MoveGenerator.h"
#include <string>
#include <vector>
// This is the engine class implementation
Engine::Engine()
{
	board = Board();
}

void Engine::NewGame()
{
	board.Clear();
}

void Engine::SetBoard(const Board &newBoard)
{
	board = newBoard;
	board.UpdateColorBoards();
}

void Engine::MakeMove(const Move &move)
{
	gameHistory.push_back(board);
	board.MakeMove(move);
}

void Engine::UndoLastMove()
{
	board = gameHistory.back();
	gameHistory.pop_back();
}

std::string Engine::ShowBoard()
{
	return board.ShowBoard();
}

void Engine::GetPseudoLegalMoves(std::vector<Move> &pseudoLegalMoves)
{
	MoveGenerator::GetPseudoLegalMoves(*this, pseudoLegalMoves);
}

Move Engine::GetBestMove()
{
	std::vector<Move> legalmoves;
	GetLegalMoves(legalmoves);
	return legalmoves[0];
}

void Engine::GetLegalMoves(std::vector<Move> &legalMoves)
{
	GetPseudoLegalMoves(legalMoves);
}

int Engine::Perft(const int &depth)
{
	std::vector<Move> legalMoves;
	GetLegalMoves(legalMoves);
	if (depth <= 0)
	{
		return (int)legalMoves.size();
	}
	int numberOfLeafs = 0;
	for (const auto &current : legalMoves)
	{
		MakeMove(current);
		numberOfLeafs += Perft(depth - 1);
		UndoLastMove();
	}
	return numberOfLeafs;
}