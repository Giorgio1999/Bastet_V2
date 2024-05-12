#include "Engine.h"
#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "BitBoardUtility.h"
#include <string>
#include <vector>
#include <iostream>

// This is the engine class implementation
Engine::Engine()
{
	board = Board();
	stopFlag = false;
}

void Engine::Boot(){
	ComputeMasks();
	MoveGenerator::PreComputeMoves();
}

void Engine::NewGame()
{
	board.Clear();
}

void Engine::SetBoard(const Board &newBoard)
{
	board = newBoard;
	board.UpdateColorBoards();
	board.InitialiseKingIndices();
}

void Engine::MakeMove(const Move &move)
{
	gameHistory.push_back(board);
	board.MakeMove(move);
}

void Engine::MakeSimpleMove(const Move& move){
	gameHistory.push_back(board);
	board.MakeSimpleMove(move);
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
	MoveGenerator::GetLegalMoves(*this, legalMoves);
}

int Engine::Perft(const int &depth)
{
	std::vector<Move> legalMoves;
	GetLegalMoves(legalMoves);
	// if (depth == 1)
	// {
	// 	return legalMoves.size();
	// }
	if(depth==0){
		return 1;
	}
	int numberOfLeafs = 0;
	int newDepth = depth - 1;
	for (const auto &current : legalMoves)
	{
		MakeMove(current);
		if(!stopFlag){
			numberOfLeafs += Perft(newDepth);
		}
		UndoLastMove();
	}
	return numberOfLeafs;
}