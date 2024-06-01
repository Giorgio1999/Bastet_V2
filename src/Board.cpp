#include "Board.h"
#include "BitBoardUtility.h"
#include "BoardUtility.h"
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

void Board::MakeMove(const move &move, bitboard &zobristKey)
{
	// Update nonreversible moves
	nonReversibleMoves++;

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
			zobristKey ^= hashes[startPiece * startIndex];
			zobristKey ^= hashes[startPiece * targetIndex];
			break;
		}
	}

	if (startPiece == colorIndex)
	{
		nonReversibleMoves = 0;
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
			zobristKey ^= hashes[targetPiece * targetIndex];
			nonReversibleMoves = 0;
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
			zobristKey ^= hashes[12 * 16 + 5 + ((startIndex - 8) >> 3)];
		}
		else if (diff == -16)
		{
			ghostBoard |= start << 8;
			zobristKey ^= hashes[12 * 16 + 5 + ((startIndex + 8) >> 3)];
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
			zobristKey &= ~hashes[otherColorIndex * (targetIndex + 8)];
		}
		else
		{
			pieceBoards[otherColorIndex] &= ~(target >> 8);
			colorBoards[color] &= ~(target >> 8);
			zobristKey &= ~hashes[otherColorIndex * (targetIndex - 8)];
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
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex - 1)];
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex + 1)];
		}
		castleTarget = color ? 58 : 2;
		if (targetIndex == castleTarget)
		{
			pieceBoards[3 + colorIndex] ^= target << 1 | target >> 2;
			colorBoards[!color] ^= target << 1 | target >> 2;
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex + 1)];
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex - 2)];
		}
		nonReversibleMoves = 0;
	}

	// Promotions
	if (((move & 0x8000) >> 15) == 1)
	{
		pieceBoards[colorIndex] ^= target;
		colorBoards[!color] ^= target;
		pieceBoards[colorIndex + ((move & 0x7000) >> 12)] ^= target;
		colorBoards[!color] ^= target;
		zobristKey ^= hashes[(colorIndex + ((move & 0x7000) >> 12)) * targetIndex];
		zobristKey ^= hashes[colorIndex * targetIndex];
	}

	// Update castling flags
	flags &= (startIndex == 63 || targetIndex == 63 || startIndex == 60) ? 0b00011101 : flags;
	flags &= (startIndex == 56 || targetIndex == 56 || startIndex == 60) ? 0b00011011 : flags;
	flags &= (startIndex == 7 || targetIndex == 7 || startIndex == 4) ? 0b00010111 : flags;
	flags &= (startIndex == 0 || targetIndex == 0 || startIndex == 4) ? 0b00001111 : flags;
	zobristKey ^= (startIndex == 63 || targetIndex == 63 || startIndex == 60) ? hashes[12 * 64 + 1] : ZERO;
	zobristKey ^= (startIndex == 56 || targetIndex == 56 || startIndex == 60) ? hashes[12 * 64 + 2] : ZERO;
	zobristKey ^= (startIndex == 7 || targetIndex == 7 || startIndex == 4) ? hashes[12 * 64 + 3] : ZERO;
	zobristKey ^= (startIndex == 0 || targetIndex == 0 || startIndex == 4) ? hashes[12 * 64 + 4] : ZERO;

	// Update King coords
	// kingIndices[!color] = (startPiece == (5 + colorIndex)) ? targetIndex : kingIndices[!color];

	// Update turn flag
	flags ^= ONE;
	zobristKey ^= hashes[12 * 64];
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
	// kingIndices[!color] = (startPiece == (5 + colorIndex)) ? targetIndex : kingIndices[!color];

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
// --------------------------------------------

// Visualization tool
// --------------------------------------------
std::string Board::ShowBoard()
{
	std::string boardVisual = "\n";
	for (auto x = 0; x < 8; x++)
	{
		for (auto y = 0; y < 8; y++)
		{
			auto printSymbol = '-';
			for (auto i = 0; i < 6; i++)
			{
				if (CheckBit(pieceBoards[i], y, x))
				{
					printSymbol = whiteTypes.at(i);
				}
				if (CheckBit(pieceBoards[i + 6], y, x))
				{
					printSymbol = blackTypes.at(i);
				}
				if (CheckBit(ghostBoard, y, x))
				{
					printSymbol = ((flags & 1) == 1) ? whiteTypes.at(6) : blackTypes.at(6);
				}
			}
			boardVisual += printSymbol;
		}
		boardVisual += "\n";
	}
	boardVisual += "\nFlags: ";
	boardVisual += std::to_string(flags) + " " + "\n";
	return boardVisual;
}
// --------------------------------------------