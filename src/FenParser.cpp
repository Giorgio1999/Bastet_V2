#include "FenParser.h"
#include "Piece.h"
#include "Move.h"
#include "Board.h"
#include "BitBoardUtility.h"
#include <string>
#include <cstdint>

Board Fen2Position() {
	return Fen2Position(startingPosition);
}

Board Fen2Position(std::string fen) {
	Board board;
	std::string position = fen.substr(0, fen.find_first_of(' '));
	int increment = 0;
	for (auto i = 0; i < position.length();i++) {
		char current = position[i];
		if (current != '/') {
			uint_fast64_t tmp = (uint_fast64_t)1 << increment;
			switch (current) {
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
	std::string rights = fen.substr(fen.find_first_of(' ') + 1, fen.length() - 1);
	char current = rights[0];
	switch (current) {
		case 'w':
			board.whiteToMove = true;
			break;
		case 'b':
			board.whiteToMove = false;
			break;
	}

	std::string castling = rights.substr(2, rights.length()-1);
	castling = castling.substr(0, castling.find_first_of(' '));
	for (int i = 0; i < castling.length(); i++) {
		switch (castling[i]) {
			case 'K':
				board.castlingRights[0] = true;
				break;
			case 'Q':
				board.castlingRights[1] = true;
				break;
			case 'k':
				board.castlingRights[2] = true;
				break;
			case 'q':
				board.castlingRights[3] = true;
				break;
			case '-':
				board.castlingRights[0] = false;
				board.castlingRights[1] = false;
				board.castlingRights[2] = false;
				board.castlingRights[3] = false;
				break;
		}
	}
	std::string enPassant = rights.substr(castling.length()+3, rights.length() - 1);
	enPassant = enPassant.substr(0, enPassant.find_first_of(' '));
	if (enPassant != "-") {
		Coord ghostPosition = Str2Coord(enPassant);
		SetBit(board.ghostBoard,ghostPosition.x,ghostPosition.y);
	}
	return board;
}