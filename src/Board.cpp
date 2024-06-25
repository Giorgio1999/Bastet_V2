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
	if (ghostBoard > 0)
	{
		square location = PopLsb(ghostBoard);
		zobristKey ^= hashes[12 * 16 + 1 + 4 + (location >> 3)];
	}
	ghostBoard = ZERO;

	int startIndex = move & START;			// StartIndex(move);
	int targetIndex = (move & TARGET) >> 6; // TargetIndex(move);
	bitboard start = ONE << startIndex;
	bitboard target = ONE << targetIndex;

	auto color = (flags & PTM) == 1;
	auto ncolor = !color;
	auto colorIndex = color ? WHITEPIECES : BLACKPIECES;
	auto otherColorIndex = color ? BLACKPIECES : WHITEPIECES;

	// Find Piece at start and move it to target
	int startPiece = NONE;
	for (int i = PAWN; i <= KING; i++)
	{
		if ((pieceBoards[i + colorIndex] & start) > 0)
		{
			startPiece = i + colorIndex;
			pieceBoards[startPiece] ^= start | target;
			colorBoards[ncolor] ^= start | target;
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
	int targetPiece = NONE;
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
		if (diff == NONO)
		{
			ghostBoard |= start >> NO;
			zobristKey ^= hashes[12 * 16 + 5 + ((startIndex - NO) >> 3)];
		}
		else if (diff == -NONO)
		{
			ghostBoard |= start << NO;
			zobristKey ^= hashes[12 * 16 + 5 + ((startIndex + NO) >> 3)];
		}
	}

	// Enpassant: if pawn moves to unoccupied square, remove enemy pawn at one rank below(above) target. Will only affect en passant moves
	if (startPiece == colorIndex && targetPiece < A8)
	{
		if (color)
		{
			pieceBoards[otherColorIndex] &= ~(target << NO);
			colorBoards[color] &= ~(target << NO);
			zobristKey &= ~hashes[otherColorIndex * (targetIndex + NO)];
		}
		else
		{
			pieceBoards[otherColorIndex] &= ~(target >> NO);
			colorBoards[color] &= ~(target >> NO);
			zobristKey &= ~hashes[otherColorIndex * (targetIndex - NO)];
		}
	}

	// Castling (moving the rook)
	if (startPiece == KING + colorIndex && startIndex == (color ? E1 : E8) && std::abs(startIndex - targetIndex) > 1)
	{
		int castleTarget = color ? G1 : G8;
		if (targetIndex == castleTarget)
		{
			pieceBoards[ROOK + colorIndex] ^= target >> EA | target << EA;
			colorBoards[ncolor] ^= target >> EA | target << EA;
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex - EA)];
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex + EA)];
		}
		castleTarget = color ? C1 : C8;
		if (targetIndex == castleTarget)
		{
			pieceBoards[ROOK + colorIndex] ^= target << EA | target >> EAEA;
			colorBoards[ncolor] ^= target << EA | target >> EAEA;
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex + EA)];
			zobristKey ^= hashes[(3 + colorIndex) * (targetIndex - EAEA)];
		}
		nonReversibleMoves = 0;
	}

	// Promotions
	if (((move & PROMOTION) >> 15) == 1)
	{
		pieceBoards[colorIndex] ^= target;
		colorBoards[ncolor] ^= target;
		pieceBoards[colorIndex + ((move & CNVTO) >> 12)] ^= target;
		colorBoards[ncolor] ^= target;
		zobristKey ^= hashes[(colorIndex + ((move & CNVTO) >> 12)) * targetIndex];
		zobristKey ^= hashes[colorIndex * targetIndex];
	}

	// Update castling flags
	if (startIndex == H1 || targetIndex == H1 || startIndex == E1)
	{
		flags &= nKCW;
		zobristKey ^= hashes[12 * 64 + 1];
	}
	if (startIndex == A1 || targetIndex == A1 || startIndex == E1)
	{
		flags &= nQCW;
		zobristKey ^= hashes[12 * 64 + 2];
	}
	if (startIndex == H8 || targetIndex == H8 || startIndex == E8)
	{
		flags &= nKCB;
		zobristKey ^= hashes[12 * 64 + 3];
	}
	if (startIndex == A8 || targetIndex == A8 || startIndex == E8)
	{
		flags &= nQCB;
		zobristKey ^= hashes[12 * 64 + 4];
	}

	// Update turn flag
	flags ^= PTM;
	zobristKey ^= hashes[12 * 64];
}

void Board::MakeSimpleMove(const move &move)
{
	int startIndex = move & START;			// StartIndex(move);
	int targetIndex = (move & TARGET) >> 6; // TargetIndex(move);
	bitboard start = ONE << startIndex;
	bitboard target = ONE << targetIndex;

	auto color = (flags & PTM) == 1;
	auto colorIndex = color ? WHITEPIECES : BLACKPIECES;
	auto otherColorIndex = color ? BLACKPIECES : WHITEPIECES;

	// Find Piece at start and move it to target
	int startPiece = NONE;
	for (int i = PAWN; i <= KING; i++)
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
	int targetPiece = NONE;
	for (int i = PAWN; i <= KING; i++)
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
	if (startPiece == colorIndex && targetPiece < A8)
	{
		if (color)
		{
			pieceBoards[otherColorIndex] &= ~(target << NO);
			colorBoards[color] &= ~(target << NO);
		}
		else
		{
			pieceBoards[otherColorIndex] &= ~(target >> NO);
			colorBoards[color] &= ~(target >> NO);
		}
	}

	// Update turn flag
	flags ^= ONE;
}

void Board::InitialiseColorBoards()
{
	colorBoards[0] = 0;
	colorBoards[1] = 0;
	for (int i = PAWN; i <= KING; i++)
	{
		colorBoards[0] |= pieceBoards[i];
		colorBoards[1] |= pieceBoards[i + BLACKPIECES];
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
					printSymbol = ((flags & PTM) == 1) ? whiteTypes.at(6) : blackTypes.at(6);
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