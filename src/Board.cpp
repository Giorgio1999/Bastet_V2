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
	bitboard start = ONE << startIndex;
	bitboard target = ONE << targetIndex;

	auto color = (flags & 1) == 1;
	auto colorIndex = color ? 0 : 6;
	auto otherColorIndex = color ? 6 : 0;
	auto colorDirection = color ? -1 : 1;

	// Find Piece at start and move it to target
	int startPiece = 0;
	for (int i = 0; i < 6; i++)
	{
		if ((pieceBoards[i + colorIndex] & start) > 0)
		{
			startPiece = i + colorIndex;
			pieceBoards[startPiece] ^= start | target;
			colorBoards[!color] ^= start|target;
			break;
		}
	}

	// Find Piece at target and remove it
	int targetPiece = -1;
	for (int i = 0; i < 6; i++)
	{
		if ((pieceBoards[i + otherColorIndex] & target) > 0)
		{
			targetPiece = i + otherColorIndex;
			pieceBoards[targetPiece] ^= target;
			colorBoards[color] ^=target;
			break;
		}
	}

	// Pawn double pushes. leaves ghost
	if (startPiece == colorIndex)
	{
		auto diff = startIndex - targetIndex;
		if (diff == 16)
		{
			ghostBoard |= start >> 8;
		}
		else if (diff == -16)
		{
			ghostBoard |= start << 8;
		}
	}

	// Enpassant: if pawn moves to unoccupied square, remove enemy pawn at one rank below(above) target. Will only affect en passant moves
	if (startPiece == colorIndex && targetPiece < 0)
	{
		// UnsetBit(pieceBoards[otherColorIndex], targetIndex - colorDirection * 8);
		if (color)
		{
			pieceBoards[otherColorIndex] &= ~(target << 8);
			colorBoards[color] &= ~(target << 8);
		}
		else
		{
			pieceBoards[otherColorIndex] &= ~(target >> 8);
			colorBoards[color] &= ~(target >> 8);
		}
	}

	// Castling (moving the rook)
	if (startPiece == 5 + colorIndex && startIndex == (color ? 60 : 4) && std::abs(startIndex - targetIndex) > 1)
	{
		auto castleTarget = color ? 62 : 6;
		if (targetIndex == castleTarget)
		{
			pieceBoards[3 + colorIndex] ^= target >> 1 | target << 1;
			colorBoards[!color] ^= target >> 1 | target << 1;
		}
		castleTarget = color ? 58 : 2;
		if (targetIndex == castleTarget)
		{
			pieceBoards[3 + colorIndex] ^= target << 1 | target >> 2;
			colorBoards[!color] ^= target << 1 | target >> 2;
		}
	}

	// Promotions
	if (Promotion(move) == 1)
	{
		pieceBoards[colorIndex] ^= target;
		colorBoards[!color] ^= target;
		pieceBoards[colorIndex+ConvertTo(move)] ^= target;
		colorBoards[!color] ^=target;
	}

	// Update castling flags
	flags &= (startIndex == 63 || targetIndex == 63 || startIndex == 60) ? 0b00011101 : 31;
	flags &= (startIndex == 56 || targetIndex == 56 || startIndex == 60) ? 0b00011011 : 31;
	flags &= (startIndex == 7 || targetIndex == 7 || startIndex == 4) ? 0b00010111 : 31;
	flags &= (startIndex == 0 || targetIndex == 0 || startIndex == 4) ? 0b00001111 : 31;

	// Update King coords
	kingIndices[!color] = (startPiece==(5+colorIndex))?targetIndex:kingIndices[!color];
	kingIndices[color] = (startPiece==(5+otherColorIndex))?targetIndex:kingIndices[color];


	// Update turn flag
	flags ^= ONE;
}

void Board::MakeSimpleMove(const move &move)
{
		// Clear ghosts
	ghostBoard = ZERO;

	int startIndex = StartIndex(move);
	int targetIndex = TargetIndex(move);
	bitboard start = ONE << startIndex;
	bitboard target = ONE << targetIndex;

	auto color = (flags & 1) == 1;
	auto colorIndex = color ? 0 : 6;
	auto otherColorIndex = color ? 6 : 0;
	auto colorDirection = color ? -1 : 1;

	// Find Piece at start and move it to target
	int startPiece = 0;
	for (int i = 0; i < 6; i++)
	{
		if ((pieceBoards[i + colorIndex] & start) > 0)
		{
			startPiece = i + colorIndex;
			pieceBoards[startPiece] ^= start | target;
			colorBoards[!color] ^= start|target;
			break;
		}
	}

	// Find Piece at target and remove it
	int targetPiece = -1;
	for (int i = 0; i < 6; i++)
	{
		if ((pieceBoards[i + otherColorIndex] & target) > 0)
		{
			targetPiece = i + otherColorIndex;
			pieceBoards[targetPiece] ^= target;
			colorBoards[color] ^=target;
			break;
		}
	}

	// Enpassant: if pawn moves to unoccupied square, remove enemy pawn at one rank below(above) target. Will only affect en passant moves
	if (startPiece == colorIndex && targetPiece < 0)
	{
		// UnsetBit(pieceBoards[otherColorIndex], targetIndex - colorDirection * 8);
		if (color)
		{
			pieceBoards[otherColorIndex] &= ~(target << 8);
			colorBoards[color] &= ~(target << 8);
		}
		else
		{
			pieceBoards[otherColorIndex] &= ~(target >> 8);
			colorBoards[color] &= ~(target >> 8);
		}
	}

	// Update King coords
	kingIndices[!color] = (startPiece==(5+colorIndex))?targetIndex:kingIndices[!color];
	kingIndices[color] = (startPiece==(5+otherColorIndex))?targetIndex:kingIndices[color];


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
	kingIndices[0] = BitScanForwards(pieceBoards[5]) - 1;
	kingIndices[1] = BitScanForwards(pieceBoards[11]) - 1;
}

void Board::UpdateKingIndices(const int &startIndex, const int &targetIndex)
{
	kingIndices[!((flags & 1) == 1)] = kingIndices[!((flags & 1) == 1)] == startIndex ? targetIndex : kingIndices[!((flags & 1) == 1)];
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
