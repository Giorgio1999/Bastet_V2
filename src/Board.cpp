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

void Board::MakeMove(const Move &move)
{
	// Clear ghosts
	ghostBoard = ZERO;

	int startIndex = move.startIndex;
	int targetIndex = move.targetIndex;

	auto wasCastled = false; // Castling flag

	auto color = whiteToMove ? 0 : 6;
	auto otherColor = whiteToMove ? 6 : 0;
	auto colorDirection = whiteToMove ? -1 : 1;
	for (auto i = color; i < 6 + color; i++)
	{ // go trough all pieces of the color to move and find the piece at start
		if (i == color)
		{
			// Pawn double pushes. leaves ghost
			if (CheckBit(pieceBoards[i], startIndex))
			{
				if (std::abs(startIndex - targetIndex) == 16)
				{
					SetBit(ghostBoard, startIndex + colorDirection * 8);
				}
			}
			// Enpassant
			if (CheckBit(pieceBoards[i], startIndex) && !CheckBit(colorBoards[whiteToMove], targetIndex))
			{
				if (std::abs(startIndex - targetIndex) == 9 || std::abs(startIndex - targetIndex) == 7)
				{
					UnsetBit(pieceBoards[otherColor], targetIndex - colorDirection * 8);
				}
			}
		}
		// if(i == color +5){
		// 	wasCastled = startIndex == (whiteToMove ? 60 : 4) && std::abs(startIndex - targetIndex) > 1; // flag if move was a castle
		// }

		// Generic moving
		// pieceBoards[i] |= ((pieceBoards[i] >> startIndex) & 1) << targetIndex;
		// UnsetBit(pieceBoards[i],startIndex);
		if (CheckBit(pieceBoards[i], startIndex))
		{ // is piece found, set piece bitboard at target and unset at start
			SetBit(pieceBoards[i], targetIndex);
			UnsetBit(pieceBoards[i], startIndex);
			wasCastled = (i == 5 + color) && startIndex == (whiteToMove ? 60 : 4) && std::abs(startIndex - targetIndex) > 1; // flag if move was a castle
			break;
		}
	}
	color = whiteToMove ? 6 : 0;
	for (auto i = 0; i < 6; i++)
	{ // for all not to move pieces, find piece at target and unset
		UnsetBit(pieceBoards[i + color], targetIndex);
	}

	color = whiteToMove ? 0 : 6;

	// Castling (moving the rook)
	if (wasCastled)
	{
		auto castleTarget = whiteToMove ? 62 : 6;
		if (targetIndex == castleTarget)
		{
			// SetBit(pieceBoards[3 + color], 5, whiteToMove ? 7 : 0);
			SetBit(pieceBoards[3 + color], targetIndex - 1);
			// UnsetBit(pieceBoards[3 + color], 7, whiteToMove ? 7 : 0);
			UnsetBit(pieceBoards[3 + color], targetIndex + 1);
		}
		castleTarget = whiteToMove ? 58 : 2;
		if (targetIndex == castleTarget)
		{
			// SetBit(pieceBoards[3 + color], 3, whiteToMove ? 7 : 0);
			SetBit(pieceBoards[3 + color], targetIndex + 1);
			// UnsetBit(pieceBoards[3 + color], 0, whiteToMove ? 7 : 0);
			UnsetBit(pieceBoards[3 + color], targetIndex - 2);
		}
	}

	// Promotions
	if (move.promotion)
	{
		UnsetBit(pieceBoards[color], targetIndex);
		SetBit(pieceBoards[color + move.convertTo], targetIndex);
	}

	// Update castling flags
	// if ((start.x == 7 && start.y == 7) || (target.x == 7 && target.y == 7))
	if (startIndex == 63 || targetIndex == 63) // 7,7
	{
		castlingRights[0] = false;
	}
	if (startIndex == 56 || targetIndex == 56) // 0,7
	{
		castlingRights[1] = false;
	}
	if (startIndex == 7 || targetIndex == 7) // 7,0
	{
		castlingRights[2] = false;
	}
	if (startIndex == 0 || targetIndex == 0) // 0,0
	{
		castlingRights[3] = false;
	}
	if (startIndex == 60) // 4,7
	{
		castlingRights[0] = false;
		castlingRights[1] = false;
	}
	if (startIndex == 4) // 4,0
	{
		castlingRights[2] = false;
		castlingRights[3] = false;
	}

	// TO DO: Maybe check for checks?

	// Update Color Boards
	UpdateColorBoards();

	// Update King coords
	UpdateKingIndices(move);

	// Update turn flag
	whiteToMove = !whiteToMove;
}

void Board::MakeSimpleMove(const Move &move)
{
	// Clear ghosts
	ghostBoard = ZERO;

	int startIndex = move.startIndex;
	int targetIndex = move.targetIndex;

	auto color = whiteToMove ? 0 : 6;
	auto otherColor = whiteToMove ? 6 : 0;
	auto colorDirection = whiteToMove ? -1 : 1;
	for (auto i = color; i < 6 + color; i++)
	{ // go trough all pieces of the color to move and find the piece at start
		if (i == color)
		{
			// Enpassant
			if (CheckBit(pieceBoards[i], startIndex) && !CheckBit(colorBoards[whiteToMove], targetIndex))
			{
				if (std::abs(startIndex - targetIndex) == 9 || std::abs(startIndex - targetIndex) == 7)
				{
					UnsetBit(pieceBoards[otherColor], targetIndex - colorDirection * 8);
				}
			}
		}
		// Generic moving
		if (CheckBit(pieceBoards[i], startIndex))
		{ // is piece found, set piece bitboard at target and unset at start
			SetBit(pieceBoards[i], targetIndex);
			UnsetBit(pieceBoards[i], startIndex);
			break;
		}
	}
	color = whiteToMove ? 6 : 0;
	for (auto i = 0; i < 6; i++)
	{ // for all not to move pieces, find piece at target and unset
		UnsetBit(pieceBoards[i + color], targetIndex);
	}

	color = whiteToMove ? 0 : 6;

	// TO DO: Maybe check for checks?

	// Update Color Boards
	UpdateColorBoards();

	// Update King coords
	UpdateKingIndices(move);

	// Update turn flag
	whiteToMove = !whiteToMove;
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

void Board::UpdateKingIndices(const Move &move)
{
	kingIndices[!whiteToMove] = kingIndices[!whiteToMove] == move.startIndex ? move.targetIndex : kingIndices[!whiteToMove];
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
	for (const auto right : castlingRights)
	{
		boardVisual += std::to_string(right) + " ";
	}
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
