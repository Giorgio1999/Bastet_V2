#include "Engine.h"
#include "Board.h"
#include "BoardUtility.h"
#include "MoveGenerator.h"
#include "BitBoardUtility.h"
#include "Timer.h"
#include "Search.h"
#include "MathUtility.h"
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
	MathUtility::Random<bitboard> prng((bitboard)2938472947865982);
	for (uint i = 0; i < 781; i++)
	{
		hashes[i] = prng.Next();
	}
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
	for (uint i = 0; i < 12; i++)
	{
		bitboard pieceBoard = newBoard.pieceBoards[i];
		square location = PopLsb(pieceBoard);
		currentZobristKey ^= hashes[i * location];
	}
	if ((newBoard.flags & 1) == 1)
	{
		currentZobristKey ^= hashes[12 * 16];
	}
	if ((newBoard.flags & 0b00000010) > 0)
	{
		currentZobristKey ^= hashes[12 * 16 + 1];
	}
	if ((newBoard.flags & 0b00000100) > 0)
	{
		currentZobristKey ^= hashes[12 * 16 + 2];
	}
	if ((newBoard.flags & 0b00001000) > 0)
	{
		currentZobristKey ^= hashes[12 * 16 + 3];
	}
	if ((newBoard.flags & 0b00010000) > 0)
	{
		currentZobristKey ^= hashes[12 * 16 + 4];
	}
	bitboard ghostBoard = newBoard.ghostBoard;
	square location = PopLsb(ghostBoard);
	currentZobristKey ^= hashes[12*16+1+4+(location>>3)];
	repetitionTable.push_back(currentZobristKey);
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
	CurrentBoard().MakeMove(move,currentZobristKey);
	repetitionTable.push_back(currentZobristKey);
}

void Engine::MakeSimpleMove(const move &move)
{
	std::memcpy(&gameHistory[gameHistoryIndex + 1], &gameHistory[gameHistoryIndex], sizeof(Board));
	gameHistoryIndex++;
	CurrentBoard().MakeSimpleMove(move);
}

void Engine::MakePermanentMove(const move &move)
{
	CurrentBoard().MakeMove(move,currentZobristKey);
	repetitionTable.push_back(currentZobristKey);
}

void Engine::UndoLastMove()
{
	gameHistoryIndex--;
	repetitionTable.pop_back();
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
	return CurrentBoard().ShowBoard() + "\n" + std::to_string(currentZobristKey) + "\n";
}
// --------------------------------------------------------------------------------------------
