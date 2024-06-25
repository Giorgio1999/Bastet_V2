#include "Engine.h"
#include "Board.h"
#include "BoardUtility.h"
#include "MoveGenerator.h"
#include "BitBoardUtility.h"
#include "Timer.h"
#include "Search.h"
#include "MathUtility.h"
#include "Evaluation.h"
#include "TranspositionTable.h"
#include <string>
#include <cstring>
#include <iostream>
#include <cassert>

// This is the engine class implementation
// Initialising and accessing the engine
// --------------------------------------------------------------------------------------------
Engine::Engine()
{
	gameHistoryIndex = 0;
	CurrentBoard() = Board();
	stopFlag = false;
	prng = new MathUtility::Random<int>(2350927498);
}

void Engine::Boot()
{
	ComputeMasks();
	MoveGenerator::PreComputeMoves();
	ComputeHashes();
	tt = transposition::Tt(ttSize);
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
	currentZobristKey = InitialiseZobristHash();
}

bitboard Engine::InitialiseZobristHash()
{
	Board newBoard = CurrentBoard();
	bitboard zobrist = ZERO;
	for (uint i = 0; i < 12; i++)
	{
		bitboard pieceBoard = newBoard.pieceBoards[i];
		while (pieceBoard > 0)
		{
			square location = PopLsb(pieceBoard);
			zobrist ^= hashes[i * location];
		}
	}
	if ((newBoard.flags & 1) == 1)
	{
		zobrist ^= hashes[12 * 16];
	}
	if ((newBoard.flags & 0b00000010) > 0)
	{
		zobrist ^= hashes[12 * 16 + 1];
	}
	if ((newBoard.flags & 0b00000100) > 0)
	{
		zobrist ^= hashes[12 * 16 + 2];
	}
	if ((newBoard.flags & 0b00001000) > 0)
	{
		zobrist ^= hashes[12 * 16 + 3];
	}
	if ((newBoard.flags & 0b00010000) > 0)
	{
		zobrist ^= hashes[12 * 16 + 4];
	}
	bitboard ghostBoard = newBoard.ghostBoard;
	if (ghostBoard > 0)
	{
		square location = PopLsb(ghostBoard);
		zobrist ^= hashes[12 * 16 + 1 + 4 + (location >> 3)];
	}
	return zobrist;
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
	repetitionTable.push_back(currentZobristKey);
	CurrentBoard().MakeMove(move, currentZobristKey);
}

void Engine::MakeSimpleMove(const move &move)
{
	std::memcpy(&gameHistory[gameHistoryIndex + 1], &gameHistory[gameHistoryIndex], sizeof(Board));
	gameHistoryIndex++;
	CurrentBoard().MakeSimpleMove(move);
}

void Engine::MakePermanentMove(const move &move)
{
	repetitionTable.push_back(currentZobristKey);
	CurrentBoard().MakeMove(move, currentZobristKey);
}

void Engine::UndoLastMove()
{
	gameHistoryIndex--;
	currentZobristKey = repetitionTable.back();
	repetitionTable.pop_back();
}

void Engine::UndoLastSimpleMove()
{
	gameHistoryIndex--;
}

void Engine::GetLegalMoves(std::array<move, 256> &moveHolder, uint &moveHolderIndex, bool capturesOnly)
{
	if (capturesOnly)
	{
		MoveGenerator::GetLegalCaptures(*this, moveHolder, moveHolderIndex);
	}
	else
	{
		MoveGenerator::GetLegalMoves(*this, moveHolder, moveHolderIndex);
	}
}

bool Engine::IsCheck()
{
	Board currentBoard = CurrentBoard();
	auto color = (currentBoard.flags & 1) == 1;
	auto colorIndex = color ? 0 : 6;
	int kingIndex = BitScanForwards(currentBoard.pieceBoards[5 + colorIndex]) - 1;
	return MoveGenerator::IsSquareAttacked(*this, kingIndex, !color);
}
// --------------------------------------------------------------------------------------------

// Search
// --------------------------------------------------------------------------------------------
Mover Engine::GetBestMove(Timer &timer)
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
	GetLegalMoves(moveHolder, moveHolderIndex, false);
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

float Engine::Evaluate()
{
	auto color = (CurrentBoard().flags & 1) == 1;
	auto colorMultipliyer = color ? 1 : -1;
	return colorMultipliyer * Evaluation::StaticEvaluation(*this) / (float)pieceValues[0];
}

void Engine::HashTest(int depth)
{
	std::array<move, 256> moveHolder;
	uint moveHolderIndex = 0;
	GetLegalMoves(moveHolder, moveHolderIndex, false);
	if (depth <= 1)
	{
		return;
	}
	for (uint i = 0; i < moveHolderIndex; i++)
	{
		MakeMove(moveHolder[i]);
		bitboard newHash = InitialiseZobristHash();
		assert(newHash==currentZobristKey);
		if (!stopFlag)
		{
			HashTest(depth - 1);
		}
		UndoLastMove();
	}
}
// --------------------------------------------------------------------------------------------
