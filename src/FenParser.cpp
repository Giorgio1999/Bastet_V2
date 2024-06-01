#include "FenParser.h"
#include "BoardUtility.h"
#include "Board.h"
#include "BitBoardUtility.h"
#include <string>
#include <cstdint>

// Fen parsing
// -------------------------------------------------------------------
Board Fen2Position()
{
	return Fen2Position(startingPosition);
}

Board Fen2Position(const std::string &fen)
{
	Board board;
	// Piece positions
	std::string position = fen.substr(0, fen.find_first_of(' '));
	auto increment = 0;
	for (uint i = 0; i < position.length(); i++)
	{
		char current = position[i];
		if (current != '/')
		{
			bitboard tmp = ONE << increment;
			switch (current)
			{
			case 'P':
				board.pieceBoards[0] |= tmp;
				increment++;
				break;
			case 'N':
				board.pieceBoards[1] |= tmp;
				increment++;
				break;
			case 'B':
				board.pieceBoards[2] |= tmp;
				increment++;
				break;
			case 'R':
				board.pieceBoards[3] |= tmp;
				increment++;
				break;
			case 'Q':
				board.pieceBoards[4] |= tmp;
				increment++;
				break;
			case 'K':
				board.pieceBoards[5] |= tmp;
				increment++;
				break;
			case 'p':
				board.pieceBoards[6] |= tmp;
				increment++;
				break;
			case 'n':
				board.pieceBoards[7] |= tmp;
				increment++;
				break;
			case 'b':
				board.pieceBoards[8] |= tmp;
				increment++;
				break;
			case 'r':
				board.pieceBoards[9] |= tmp;
				increment++;
				break;
			case 'q':
				board.pieceBoards[10] |= tmp;
				increment++;
				break;
			case 'k':
				board.pieceBoards[11] |= tmp;
				increment++;
				break;
			case '1':
				increment++;
				break;
			case '2':
				increment += 2;
				break;
			case '3':
				increment += 3;
				break;
			case '4':
				increment += 4;
				break;
			case '5':
				increment += 5;
				break;
			case '6':
				increment += 6;
				break;
			case '7':
				increment += 7;
				break;
			case '8':
				increment += 8;
				break;
			}
		}
	}

	// Player to move
	std::string rights = fen.substr(fen.find_first_of(' ') + 1, fen.length() - 1);
	char current = rights[0];
	switch (current)
	{
	case 'w':
		board.flags |= 0b00000001;
		break;
	}

	// Castling rights
	std::string castling = rights.substr(2, rights.length() - 1);
	castling = castling.substr(0, castling.find_first_of(' '));
	for (uint i = 0; i < castling.length(); i++)
	{
		switch (castling[i])
		{
		case 'K':
			board.flags |= 0b00000010;
			break;
		case 'Q':
			board.flags |= 0b00000100;
			break;
		case 'k':
			board.flags |= 0b00001000;
			break;
		case 'q':
			board.flags |= 0b00010000;
			break;
		}
	}

	// Enpassant
	std::string enPassant = rights.substr(castling.length() + 3, rights.length() - 1);
	enPassant = enPassant.substr(0, enPassant.find_first_of(' '));
	if (enPassant != "-")
	{
		Coord ghostPosition = Str2Coord(enPassant);
		SetBit(board.ghostBoard, ghostPosition.x, ghostPosition.y);
	}

	// Number of nonreversible moves
	std::string nonReversibleMoves = rights.substr(castling.length() + enPassant.length() + 4,rights.length()-1);
	nonReversibleMoves = nonReversibleMoves.substr(0,nonReversibleMoves.find_first_of(' '));
	board.nonReversibleMoves = std::stoi(nonReversibleMoves);

	return board;
}
// -------------------------------------------------------------------