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
			zobristKey ^= hashes[startPiece * 64 + startIndex];
			zobristKey ^= hashes[startPiece * 64 + targetIndex];
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
			zobristKey ^= hashes[targetPiece * 64 + targetIndex];
			nonReversibleMoves = 0;
			break;
		}
	}

	// Enpassant: if pawn moves to unoccupied square, remove enemy pawn at one rank below(above) target. Will only affect en passant moves
	if (startPiece == colorIndex && ghostBoard > 0)
	{
		square ghostLocation = PopLsb(ghostBoard);
		zobristKey ^= hashes[ENPASSANT + (ghostLocation >> 3)];
		if (targetIndex == ghostLocation)
		{
			if (color)
			{
				pieceBoards[otherColorIndex] ^= target << NO;
				colorBoards[color] ^= target << NO;
				zobristKey ^= hashes[otherColorIndex * 64 + (targetIndex + NO)];
			}
			else
			{
				pieceBoards[otherColorIndex] ^= target >> NO;
				colorBoards[color] ^= target >> NO;
				zobristKey ^= hashes[otherColorIndex * 64 + (targetIndex - NO)];
			}
		}
	}
	else
	{
		ghostBoard = ZERO;
	}

	// Pawn double pushes. leaves ghost
	if (startPiece == colorIndex)
	{
		auto diff = startIndex - targetIndex;
		if (diff == NONO)
		{
			ghostBoard |= start >> NO;
			zobristKey ^= hashes[ENPASSANT + ((startIndex - NO) >> 3)];
		}
		else if (diff == -NONO)
		{
			ghostBoard |= start << NO;
			zobristKey ^= hashes[ENPASSANT + ((startIndex + NO) >> 3)];
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
			zobristKey ^= hashes[(ROOK + colorIndex) * 64 + (targetIndex - EA)];
			zobristKey ^= hashes[(ROOK + colorIndex) * 64 + (targetIndex + EA)];
		}
		castleTarget = color ? C1 : C8;
		if (targetIndex == castleTarget)
		{
			pieceBoards[ROOK + colorIndex] ^= target << EA | target >> EAEA;
			colorBoards[ncolor] ^= target << EA | target >> EAEA;
			zobristKey ^= hashes[(ROOK + colorIndex) * 64 + (targetIndex + EA)];
			zobristKey ^= hashes[(ROOK + colorIndex) * 64 + (targetIndex - EAEA)];
		}
		nonReversibleMoves = 0;
	}

	// Promotions
	if ((move & PROMOTION) > 0)
	{
		pieceBoards[colorIndex] ^= target;
		colorBoards[ncolor] ^= target;
		pieceBoards[colorIndex + ((move & CNVTO) >> 12)] ^= target;
		colorBoards[ncolor] ^= target;
		zobristKey ^= hashes[(colorIndex + ((move & CNVTO) >> 12)) * 64 + targetIndex];
		zobristKey ^= hashes[colorIndex * 64 + targetIndex];
	}

	// Update castling flags
	if ((startIndex == H1 || targetIndex == H1 || startIndex == E1) && (flags & KCW) > 0)
	{
		flags ^= KCW;
		zobristKey ^= hashes[KCWHASH];
	}
	if ((startIndex == A1 || targetIndex == A1 || startIndex == E1) && (flags & QCW) > 0)
	{
		flags ^= QCW;
		zobristKey ^= hashes[QCWHASH];
	}
	if ((startIndex == H8 || targetIndex == H8 || startIndex == E8) && (flags & KCB) > 0)
	{
		flags &= nKCB;
		zobristKey ^= hashes[KCBHASH];
	}
	if ((startIndex == A8 || targetIndex == A8 || startIndex == E8) && (flags & QCB) > 0)
	{
		flags &= nQCB;
		zobristKey ^= hashes[QCBHASH];
	}

	// Update turn flag
	flags ^= PTM;
	zobristKey ^= hashes[BLACKTOMOVE];
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
	if (startPiece == colorIndex && ghostBoard > 0)
	{
		square ghostLocation = PopLsb(ghostBoard);
		if (targetIndex == ghostLocation)
		{
			if (color)
			{
				pieceBoards[otherColorIndex] ^= target << NO;
				colorBoards[color] ^= target << NO;
			}
			else
			{
				pieceBoards[otherColorIndex] ^= target >> NO;
				colorBoards[color] ^= target >> NO;
			}
		}
	}
	else
	{
		ghostBoard = ZERO;
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