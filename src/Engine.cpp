#include "Engine.h"
#include "Board.h"
#include "BoardUtility.h"
#include "MoveGenerator.h"
#include "BitBoardUtility.h"
#include "Timer.h"
#include "Search.h"
#include <string>
#include <cstring>

// This is the engine class implementation
// Initialising and accessing the engine
// --------------------------------------------------------------------------------------------
Engine::Engine()
{
	gameHistoryIndex = 0;
	CurrentBoard() = Board();
	stopFlag = false;
}

void Engine::Boot()
{
	ComputeMasks();
	MoveGenerator::PreComputeMoves();
}

void Engine::NewGame()
{
	gameHistoryIndex = 0;
	CurrentBoard() = Board();
}

void Engine::SetBoard(const Board &newBoard)
{
	gameHistoryIndex = 0;
	CurrentBoard() = newBoard;
	CurrentBoard().InitialiseColorBoards();
	CurrentBoard().InitialiseKingIndices();
}

Board &Engine::CurrentBoard()
{
	return gameHistory[gameHistoryIndex];
}
// --------------------------------------------------------------------------------------------

// Core functionality
// --------------------------------------------------------------------------------------------
void Engine::MakeMove(const move &move)
{
	std::memcpy(&gameHistory[gameHistoryIndex + 1], &gameHistory[gameHistoryIndex], sizeof(Board));
	gameHistoryIndex++;
	CurrentBoard().MakeMove(move);
}

void Engine::MakeSimpleMove(const move &move)
{
	std::memcpy(&gameHistory[gameHistoryIndex + 1], &gameHistory[gameHistoryIndex], sizeof(Board));
	gameHistoryIndex++;
	CurrentBoard().MakeSimpleMove(move);
}

void Engine::MakePermanentMove(const move &move)
{
	CurrentBoard().MakeMove(move);
}

void Engine::UndoLastMove()
{
	gameHistoryIndex--;
}
void Engine::GetLegalMoves(std::array<move, 256> &moveHolder, uint &moveHolderIndex)
{
	MoveGenerator::GetLegalMoves(*this, moveHolder, moveHolderIndex);
}
// --------------------------------------------------------------------------------------------

// Search
// --------------------------------------------------------------------------------------------
Mover Engine::GetBestMove(const Timer &timer)
{
	return Move2Mover(Search::GetBestMove(*this, timer));
}
// --------------------------------------------------------------------------------------------

// Performance evaluation
// --------------------------------------------------------------------------------------------
bitboard Engine::Perft(int depth)
{
	std::array<move, 256> moveHolder;
	uint moveHolderIndex = 0;
	GetLegalMoves(moveHolder, moveHolderIndex);
	if (depth <= 1)
	{
		return moveHolderIndex;
	}
	// if(depth==0){
	// 	return 1;
	// }
	bitboard numberOfLeafs = 0;
	// int newDepth = depth - 1;
	for (uint i = 0; i < moveHolderIndex; i++)
	{
		MakeMove(moveHolder[i]);
		if (!stopFlag)
		{
			numberOfLeafs += Perft(depth - 1);
		}
		UndoLastMove();
	}
	return numberOfLeafs;
}

// Debugging
// --------------------------------------------------------------------------------------------
std::string Engine::ShowBoard()
{
	return CurrentBoard().ShowBoard();
}
// --------------------------------------------------------------------------------------------
