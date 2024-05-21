#include "Board.h"
#include "BitBoardUtility.h"
#include <string>
#include <cstdint>

// Updating the board
// --------------------------------------------
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

	int startIndex = move & 0x003F;			// StartIndex(move);
	int targetIndex = (move & 0x0FC0) >> 6; // TargetIndex(move);
	bitboard start = ONE << startIndex;
	bitboard target = ONE << targetIndex;

	auto color = (flags & 1) == 1;
	auto colorIndex = color ? 0 : 6;
	auto otherColorIndex = color ? 6 : 0;

	// Find Piece at start and move it to target
	int startPiece = 0;
	for (int i = 0; i < 6; i++)
	{
		if ((pieceBoards[i + colorIndex] & start) > 0)
		{
			startPiece = i + colorIndex;
			pieceBoards[startPiece] ^= start | target;
			colorBoards[!color] ^= start | target;
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
			colorBoards[color] ^= target;
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
	if (((move & 0x8000) >> 15) == 1)
	{
		pieceBoards[colorIndex] ^= target;
		colorBoards[!color] ^= target;
		pieceBoards[colorIndex + ((move & 0x7000) >> 12)] ^= target;
		colorBoards[!color] ^= target;
	}

	// Update castling flags
	flags &= (startIndex == 63 || targetIndex == 63 || startIndex == 60) ? 0b00011101 : flags;
	flags &= (startIndex == 56 || targetIndex == 56 || startIndex == 60) ? 0b00011011 : flags;
	flags &= (startIndex == 7 || targetIndex == 7 || startIndex == 4) ? 0b00010111 : flags;
	flags &= (startIndex == 0 || targetIndex == 0 || startIndex == 4) ? 0b00001111 : flags;

	// Update King coords
	kingIndices[!color] = (startPiece == (5 + colorIndex)) ? targetIndex : kingIndices[!color];

	// Update turn flag
	flags ^= ONE;
}

void Board::MakeSimpleMove(const move &move)
{
	int startIndex = move & 0x003F;			// StartIndex(move);
	int targetIndex = (move & 0x0FC0) >> 6; // TargetIndex(move);
	bitboard start = ONE << startIndex;
	bitboard target = ONE << targetIndex;

	auto color = (flags & 1) == 1;
	auto colorIndex = color ? 0 : 6;
	auto otherColorIndex = color ? 6 : 0;

	// Find Piece at start and move it to target
	int startPiece = 0;
	for (int i = 0; i < 6; i++)
	{
		if ((pieceBoards[i + colorIndex] & start) > 0)
		{
			startPiece = i + colorIndex;
			pieceBoards[startPiece] ^= start | target;
			colorBoards[!color] ^= start | target;
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
			colorBoards[color] ^= target;
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
	kingIndices[!color] = (startPiece == (5 + colorIndex)) ? targetIndex : kingIndices[!color];

	// Update turn flag
	flags ^= ONE;
}

void Board::InitialiseColorBoards()
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
// --------------------------------------------

// Visualization tool
// --------------------------------------------
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
// --------------------------------------------