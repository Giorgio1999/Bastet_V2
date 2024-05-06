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

	Coord start = move.startCoord;
	Coord target = move.targetCoord;

	auto wasCastled = false; // Castling flag

	auto color = whiteToMove ? 0 : 6;
	auto otherColor = whiteToMove ? 6 : 0;
	auto colorDirection = whiteToMove ? -1 : 1;
	for (auto i = color; i < 6 + color; i++)
	{ // go trough all pieces of the color to move and find the piece at start
		if (i == color)
		{
			// Pawn double pushes. leaves ghost
			if (CheckBit(pieceBoards[i], start.x, start.y))
			{
				if (std::abs(start.y - target.y) > 1)
				{
					SetBit(ghostBoard, start.x, start.y + colorDirection);
				}
			}
			// Enpassant
			if (CheckBit(pieceBoards[i], start.x, start.y) && !CheckBit(colorBoards[whiteToMove], target.x, target.y))
			{
				if (std::abs(start.x - target.x) > 0)
				{
					UnsetBit(pieceBoards[otherColor], target.x, target.y - colorDirection);
				}
			}
		}
		// Generic moving
		if (CheckBit(pieceBoards[i], start.x, start.y))
		{ // is piece found, set piece bitboard at target and unset at start
			SetBit(pieceBoards[i], target.x, target.y);
			UnsetBit(pieceBoards[i], start.x, start.y);
			wasCastled = (i == 5 + color) && std::abs(start.x - target.x) > 1; // flag if move was a castle
			break;
		}
	}
	color = whiteToMove ? 6 : 0;
	for (auto i = color; i < 6 + color; i++)
	{ // for all not to move pieces, find piece at target and unset
		if (CheckBit(pieceBoards[i], target.x, target.y))
		{
			UnsetBit(pieceBoards[i], target.x, target.y);
			break;
		}
	}

	color = whiteToMove ? 0 : 6;

	// Castling (moving the rook)
	if (wasCastled)
	{
		if (target.x == 6)
		{
			SetBit(pieceBoards[3 + color], 5, whiteToMove ? 7 : 0);
			UnsetBit(pieceBoards[3 + color], 7, whiteToMove ? 7 : 0);
		}
		if (target.x == 2)
		{
			SetBit(pieceBoards[3 + color], 3, whiteToMove ? 7 : 0);
			UnsetBit(pieceBoards[3 + color], 0, whiteToMove ? 7 : 0);
		}
	}

	// Promotions
	if (move.convertTo != none)
	{
		UnsetBit(pieceBoards[color], target.x, target.y);
		SetBit(pieceBoards[color + (int)move.convertTo - 1], target.x, target.y);
	}

	// Update castling flags
	if ((start.x == 7 && start.y == 7) || (target.x == 7 && target.y == 7))
	{
		castlingRights[0] = false;
	}
	if ((start.x == 0 && start.y == 7) || (target.x == 0 && target.y == 7))
	{
		castlingRights[1] = false;
	}
	if ((start.x == 7 && start.y == 0) || (target.x == 7 && target.y == 0))
	{
		castlingRights[2] = false;
	}
	if ((start.x == 0 && start.y == 0) || (target.x == 0 && target.y == 0))
	{
		castlingRights[3] = false;
	}
	if (start.x == 4 && start.y == 7)
	{
		castlingRights[0] = false;
		castlingRights[1] = false;
	}
	if (start.x == 4 && start.y == 0)
	{
		castlingRights[2] = false;
		castlingRights[3] = false;
	}

	// TO DO: Maybe check for checks?

	// Update Color Boards
	UpdateColorBoards();

	// Update King coords
	UpdateKingCoords(move);

	// Update turn flag
	whiteToMove = !whiteToMove;
}

void Board::MakeSimpleMove(const Move &move)
{
	ghostBoard = 0;
	Coord start = move.startCoord;
	Coord target = move.targetCoord;

	auto color = whiteToMove ? 0 : 6;
	auto otherColor = whiteToMove ? 6 : 0;
	auto colorDirection = whiteToMove ? -1 : 1;
	for (auto i = color; i < 6 + color; i++)
	{ // go trough all pieces of the color to move and find the piece at start
		if (i == color)
		{
			// Enpassant
			if (CheckBit(pieceBoards[i], start.x, start.y))
			{
				if (std::abs(start.x - target.x) > 0 && !CheckBit(colorBoards[whiteToMove], target.x, target.y))
				{
					UnsetBit(pieceBoards[otherColor], target.x, target.y - colorDirection);
				}
			}
		}
		// Generic moving
		if (CheckBit(pieceBoards[i], start.x, start.y))
		{ // is piece found, set piece bitboard at target and unset at start
			SetBit(pieceBoards[i], target.x, target.y);
			UnsetBit(pieceBoards[i], start.x, start.y);
			break;
		}
	}
	color = whiteToMove ? 6 : 0;
	for (auto i = color; i < 6 + color; i++)
	{ // for all not to move pieces, find piece at target and unset
		if (CheckBit(pieceBoards[i], target.x, target.y))
		{
			UnsetBit(pieceBoards[i], target.x, target.y);
			break;
		}
	}

	color = whiteToMove ? 0 : 6;

	// Promotions
	if (move.convertTo != none)
	{
		UnsetBit(pieceBoards[color], target.x, target.y);
		SetBit(pieceBoards[color + (int)move.convertTo-1], target.x, target.y);
	}

	// Update Color Boards
	UpdateColorBoards();

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

void Board::InitialiseKingCoords()
{
	for (auto i = 0; i < 8; i++)
	{
		for (auto j = 0; j < 8; j++)
		{
			if (CheckBit(pieceBoards[5], i, j))
			{
				kingCoords[0] = Coord(i, j);
			}
			if (CheckBit(pieceBoards[11], i, j))
			{
				kingCoords[1] = Coord(i, j);
			}
		}
	}
}

void Board::UpdateKingCoords(const Move &move)
{
	kingCoords[!whiteToMove] = kingCoords[!whiteToMove] == move.startCoord ? move.targetCoord : kingCoords[!whiteToMove];
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
