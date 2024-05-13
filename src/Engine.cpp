#include "Engine.h"
#include "Board.h"
#include "Move.h"
#include "MoveGenerator.h"
#include "BitBoardUtility.h"
#include <string>
#include <vector>
#include <iostream>
#include <cstring>

// This is the engine class implementation
Engine::Engine()
{
	gameHistory[0] = Board();
	gameHistoryIndex = 0;
	stopFlag = false;
}

void Engine::Boot()
{
	ComputeMasks();
	MoveGenerator::PreComputeMoves();
}

void Engine::NewGame()
{
	gameHistory[0] = Board();
	gameHistoryIndex = 0;
}

void Engine::SetBoard(const Board &newBoard)
{
	gameHistory[0] = newBoard;
	gameHistory[0].UpdateColorBoards();
	gameHistory[0].InitialiseKingIndices();
}

void Engine::MakeMove(const Move &move)
{
	std::memcpy(&gameHistory[gameHistoryIndex+1],&gameHistory[gameHistoryIndex],sizeof(Board));
	gameHistory[gameHistoryIndex + 1].MakeMove(move);
	gameHistoryIndex++;
}

void Engine::MakeSimpleMove(const Move &move)
{
	std::memcpy(&gameHistory[gameHistoryIndex+1],&gameHistory[gameHistoryIndex],sizeof(Board));
	gameHistory[gameHistoryIndex + 1].MakeSimpleMove(move);
	gameHistoryIndex++;
}

void Engine::UndoLastMove()
{
	gameHistoryIndex--;
}

std::string Engine::ShowBoard()
{
	return gameHistory[0].ShowBoard();
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
	if (depth == 1)
	{
		return legalMoves.size();
	}
	// if(depth==0){
	// 	return 1;
	// }
	int numberOfLeafs = 0;
	int newDepth = depth - 1;
	for (const auto &current : legalMoves)
	{
		MakeMove(current);
		if (!stopFlag)
		{
			numberOfLeafs += Perft(newDepth);
		}
		UndoLastMove();
	}
	return numberOfLeafs;
}