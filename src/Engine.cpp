#include "Engine.h"
#include "Board.h"
#include "BoardUtility.h"
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

void Engine::MakeMove(const Mover &move)
{
	std::memcpy(&gameHistory[gameHistoryIndex+1],&gameHistory[gameHistoryIndex],sizeof(Board));
	gameHistory[gameHistoryIndex + 1].MakeMove(move);
	gameHistoryIndex++;
}

void Engine::MakeSimpleMove(const Mover &move)
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

void Engine::GetPseudoLegalMoves(Mover moveHolder[320],uint& moveHolderIndex)
{
	MoveGenerator::GetPseudoLegalMoves(*this,moveHolder,moveHolderIndex);
}

Mover Engine::GetBestMove()
{
	Mover moveHolder[320];
	uint moveHolderIndex = 0;
	GetLegalMoves(moveHolder,moveHolderIndex);
	return moveHolder[0];
}

void Engine::GetLegalMoves(Mover moveHolder[320],uint& moveHolderIndex)
{
	MoveGenerator::GetLegalMoves(*this,moveHolder,moveHolderIndex);
}

int Engine::Perft(const int &depth)
{
	Mover moveHolder[320];
	uint moveHolderIndex = 0;
	GetLegalMoves(moveHolder,moveHolderIndex);
	if (depth == 1)
	{
		return moveHolderIndex;
	}
	// if(depth==0){
	// 	return 1;
	// }
	int numberOfLeafs = 0;
	int newDepth = depth - 1;
	for (uint i=0;i<moveHolderIndex;i++)
	{
		MakeMove(moveHolder[i]);
		if (!stopFlag)
		{
			numberOfLeafs += Perft(newDepth);
		}
		UndoLastMove();
	}
	return numberOfLeafs;
}