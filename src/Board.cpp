#include "Board.h"
#include "BitBoardUtility.h"
#include <string>
#include <cstdint>

Board::Board() {
}


void Board::Clear(){
	for (auto& piece : pieceBoards) {
		piece = ZERO;
	}
	for (auto& ghost : ghostBoards) {
		ghost = ZERO;
	}
}

void Board::MakeMove(Move move){
	//Clear ghosts
	for (auto& ghost : ghostBoards) {
		ghost = ZERO;
	}

	Coord start = move.startCoord;
	Coord target = move.targetCoord;

	bool wasCastled = false;	//Castling flag

	auto color = whiteToMove ? 0 : 6;
	for (auto i = color; i < 6 + color;i++) {	//go trough all pieces of the color to move and find the piece at start
		//Pawn double pushes. leaves ghost
		if (i == color && std::abs(start.y - target.y) > 1) {
			SetBit(ghostBoards[color == 0], start.x, start.y + (whiteToMove ? -1 : 1));
		}
		//Generic moving
		if (CheckBit(pieceBoards[i], start.x, start.y)) {	//is piece found, set piece bitboard at target and unset at start
			SetBit(pieceBoards[i], target.x, target.y);
			UnsetBit(pieceBoards[i], start.x, start.y);
			wasCastled = (i == 5 + color) && std::abs(start.x - target.x) > 1;	//flag if move was a castle
			break;
		}
	}
	color = whiteToMove ? 6 : 0;
	for (auto i = color;i < 6 + color;i++) {	//for all not to move pieces, find piece at target and unset
		if (CheckBit(pieceBoards[i], target.x, target.y)) {
			UnsetBit(pieceBoards[i], target.x, target.y);
			break;
		}
	}

	color = whiteToMove ? 0 : 6;

	//Castling (moving the rook)
	if (wasCastled) {
		if (target.x == 6) {
			SetBit(pieceBoards[3 + color], 5, whiteToMove ? 7 : 0);
			UnsetBit(pieceBoards[3 + color], 7, whiteToMove ? 7 : 0);
		}
		if (target.x == 2) {
			SetBit(pieceBoards[3 + color], 3, whiteToMove ? 7 : 0);
			UnsetBit(pieceBoards[3 + color], 0, whiteToMove ? 7 : 0);
		}
	}

	//Promotions
	if (move.convertTo != none) {
		UnsetBit(pieceBoards[color], target.x, target.y);
		SetBit(pieceBoards[color + (int)move.convertTo], target.x, target.y);
	}
	
	//Update castling flags
	if (start.x == 7 && start.y == 7) { castlingRights[0] = false; }
	if (start.x == 0 && start.y == 7) { castlingRights[1] = false; }
	if (start.x == 7 && start.y == 0) { castlingRights[2] = false; }
	if (start.x == 0 && start.y == 0) { castlingRights[3] = false; }
	if (start.x == 4 && start.y == 7) { castlingRights[0] = false; castlingRights[1] = false; }
	if (start.x == 4 && start.y == 0) { castlingRights[2] = false; castlingRights[3] = false; }

	//TO DO: Maybe check for checks?

	//Update turn flag
	whiteToMove = !whiteToMove;
}

std::string Board::ShowBoard() {
	std::string boardVisual = "";
	for (const auto& piece : pieceBoards) {
		for (auto i = 0;i < 8;i++) {
			for (auto j = 0;j < 8;j++) {
				boardVisual += std::to_string(CheckBit(piece,j,i));
			}
			boardVisual += "\n";
		}
		boardVisual += "\n";
		boardVisual += "\n";
	}
	boardVisual += "Castling Rights: ";
	for (const auto right : castlingRights) {
		boardVisual += std::to_string(right) + " ";
	}
	boardVisual += "\nGhost Boards:\n";
	for (const auto& ghost : ghostBoards) {
		for (auto i = 0;i < 8;i++) {
			for (auto j = 0;j < 8;j++) {
				boardVisual += std::to_string(CheckBit(ghost, j, i));
			}
			boardVisual += "\n";
		}
		boardVisual += "\n";
		boardVisual += "\n";
	}
	return boardVisual;
}
