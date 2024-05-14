#include "Board.h"
#include "BitBoardUtility.h"
#include <string>
#include <cstdint>

Board::Board()
{
}

void Board::Clear()
{
	for (auto &piece : pieceBoards)
	{
		piece = ZERO;
	}
	ghostBoard = ZERO;
}

void Board::MakeMove(const move &move)
{
	// Clear ghosts
	ghostBoard = ZERO;

	int startIndex = StartIndex(move);
	int targetIndex = TargetIndex(move);

	auto wasCastled = false; // Castling flag

	auto color = (flags&1)==1;
	auto colorIndex = color ? 0 : 6;
	auto otherColorIndex = color ? 6 : 0;
	auto colorDirection = color ? -1 : 1;

	// Pawn double pushes. leaves ghost
	if (CheckBit(pieceBoards[colorIndex], startIndex))
	{
		if (std::abs(startIndex - targetIndex) == 16)
		{
			SetBit(ghostBoard, startIndex + colorDirection * 8);
		}
	}
	// Enpassant
	if (CheckBit(pieceBoards[colorIndex], startIndex) && !CheckBit(colorBoards[color], targetIndex))
	{
		if (std::abs(startIndex - targetIndex) == 9 || std::abs(startIndex - targetIndex) == 7)
		{
			UnsetBit(pieceBoards[otherColorIndex], targetIndex - colorDirection * 8);
		}
	}

	for (auto i = colorIndex; i < 6 + colorIndex; i++)
	{ // go trough all pieces of the color to move and find the piece at start
		// Generic moving
		if (CheckBit(pieceBoards[i], startIndex))
		{ // is piece found, set piece bitboard at target and unset at start
			SetBit(pieceBoards[i], targetIndex);
			UnsetBit(pieceBoards[i], startIndex);
			wasCastled = (i == 5 + colorIndex) && startIndex == (color ? 60 : 4) && std::abs(startIndex - targetIndex) > 1; // flag if move was a castle
			break;
		}
	}

	for (auto i = 0; i < 6; i++)
	{ // for all not opposite color pieces, find piece at target and unset
		UnsetBit(pieceBoards[i + otherColorIndex], targetIndex);
	}

	// Castling (moving the rook)
	if (wasCastled)
	{
		auto castleTarget = color ? 62 : 6;
		if (targetIndex == castleTarget)
		{
			SetBit(pieceBoards[3 + colorIndex], targetIndex - 1);
			UnsetBit(pieceBoards[3 + colorIndex], targetIndex + 1);
		}
		castleTarget = color ? 58 : 2;
		if (targetIndex == castleTarget)
		{
			SetBit(pieceBoards[3 + colorIndex], targetIndex + 1);
			UnsetBit(pieceBoards[3 + colorIndex], targetIndex - 2);
		}
	}

	// Promotions
	if (Promotion(move)==1)
	{
		UnsetBit(pieceBoards[colorIndex], targetIndex);
		SetBit(pieceBoards[colorIndex + ConvertTo(move)], targetIndex);
	}

	// Update castling flags
	flags &= (startIndex == 63 || targetIndex == 63 || startIndex == 60)?0b00011101:31;
	flags &= (startIndex == 56 || targetIndex == 56 || startIndex == 60)?0b00011011:31;
	flags &= (startIndex == 7 || targetIndex == 7 || startIndex == 4)?0b00010111:31;
	flags &= (startIndex == 0 || targetIndex == 0 || startIndex == 4)?0b00001111:31;

	// Update Color Boards
	UpdateColorBoards();

	// Update King coords
	UpdateKingIndices(startIndex,targetIndex);

	// Update turn flag
	flags ^= ONE;
}

void Board::MakeSimpleMove(const move &move)
{
	// Clear ghosts
	ghostBoard = ZERO;

	int startIndex = StartIndex(move);
	int targetIndex = TargetIndex(move);

	auto color = (flags&1)==1;
	auto colorIndex = color ? 0 : 6;
	auto otherColorIndex = color ? 6 : 0;
	auto colorDirection = color ? -1 : 1;
	// Enpassant
	if (CheckBit(pieceBoards[colorIndex], startIndex) && !CheckBit(colorBoards[color], targetIndex))
	{
		if (std::abs(startIndex - targetIndex) == 9 || std::abs(startIndex - targetIndex) == 7)
		{
			UnsetBit(pieceBoards[otherColorIndex], targetIndex - colorDirection * 8);
		}
	}
	for (auto i = colorIndex; i < 6 + colorIndex; i++)
	{ // go trough all pieces of the color to move and find the piece at start
		// Generic moving
		if (CheckBit(pieceBoards[i], startIndex))
		{ // is piece found, set piece bitboard at target and unset at start
			SetBit(pieceBoards[i], targetIndex);
			UnsetBit(pieceBoards[i], startIndex);
			break;
		}
	}

	for (auto i = 0; i < 6; i++)
	{ // for all not to move pieces, find piece at target and unset
		UnsetBit(pieceBoards[i + otherColorIndex], targetIndex);
	}

	// TO DO: Maybe check for checks?

	// Update Color Boards
	UpdateColorBoards();

	// Update King coords
	UpdateKingIndices(startIndex,targetIndex);

	// Update turn flag
	flags ^= ONE;
}

void Board::UpdateColorBoards()
{
	colorBoards[0] = 0;
	colorBoards[1] = 0;
	for (auto i = 0; i < 6; i++)
	{
		colorBoards[0] |= pieceBoards[i];
		colorBoards[1] |= pieceBoards[i + 6];
	}
}

void Board::InitialiseKingIndices()
{
	kingIndices[0] = BitScanForwards(pieceBoards[5])-1;
	kingIndices[1] = BitScanForwards(pieceBoards[11])-1;
}

void Board::UpdateKingIndices(const int& startIndex, const int&targetIndex)
{
	kingIndices[!((flags&1)==1)] = kingIndices[!((flags&1)==1)] == startIndex ? targetIndex : kingIndices[!((flags&1)==1)];
}

std::string Board::ShowBoard()
{
	std::string boardVisual = "";
	for (const auto &piece : pieceBoards)
	{
		for (auto i = 0; i < 8; i++)
		{
			for (auto j = 0; j < 8; j++)
			{
				boardVisual += std::to_string(CheckBit(piece, j, i));
			}
			boardVisual += "\n";
		}
		boardVisual += "\n";
		boardVisual += "\n";
	}
	boardVisual += "Castling Rights: ";
	boardVisual += std::to_string(flags) + " ";
	boardVisual += "\nGhost Boards:\n";
	for (auto i = 0; i < 8; i++)
	{
		for (auto j = 0; j < 8; j++)
		{
			boardVisual += std::to_string(CheckBit(ghostBoard, j, i));
		}
		boardVisual += "\n";
	}
	return boardVisual;
}
