#pragma once
#include "Board.h"
#include "Piece.h"
#include "Move.h"
#include <string>
#include <vector>

//This will include the core functionality

class Engine {
	public:
		std::vector<Board> gameHistory;	//History of the game, consisting of boards. For undo or perhaps draw checks?
		Board board;	//Board representation
		Engine();
		void NewGame();						//clears board for new game
		void SetBoard(Board board);	//Sets a given PieceList as position
		void MakeMove(Move move);			//Make a move
		void UndoLastMove();				//Reverts to previous position
		std::string ShowBoard();			//Debug tool to display current board in console

		void GetPseudoLegalMoves(std::vector<Move>& moveVector);	//Gets Pseudo legal moves and writes them into the given vector
		void GetPseudoLegalPawnMoves(std::vector<Move>& moveVector);	//function for every piecetype
		void GetPseudoLegalKnightMoves(std::vector<Move>& moveVector);
		void GetPseudoLegalBishopMoves(std::vector<Move>& moveVector);
		void GetPseudoLegalRookMoves(std::vector<Move>& moveVector);
		void GetPseudoLegalQueenMoves(std::vector<Move>& moveVector);
		void GetPseudoLegalKingMoves(std::vector<Move>& moveVector);
		
		void GetLegalMoves(std::vector<Move>& moveVector);	//Filters legal moves out of pseudolegal moves

		Move GetBestMove();

		int Perft(int depth);
};