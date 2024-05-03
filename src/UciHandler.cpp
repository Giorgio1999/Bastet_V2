#include "UciHandler.h"
#include "EngineController.h"
#include <iostream>
#include <string>

void Listen() {
	EngineController engineController = EngineController();
	std::string instruction = "";
	while (std::getline(std::cin, instruction)) {
		std::string key = instruction.substr(0, instruction.find(' '));
		if (key == "uci") {
			std::cout << "id name Bastet\n";
			std::cout << "id author G. Lovato\n";
			std::cout << "uciok\n";
			engineController.BootEngine();
		}
		if (key == "isready") {
			while (!engineController.IsReady())
			{
				engineController.TestReady();
			}
			std::cout << "readyok\n";
		}
		if (key == "ucinewgame") {
			engineController.NewGame();
			//std::cout << "setting up new game\n";
		}
		if (key == "position" && instruction.length()!=key.length()) {
			std::string arguments = instruction.substr(key.length()+1, instruction.length());
			std::string positionKey = arguments.substr(0, arguments.find(' '));
			if (positionKey == "startpos") {
				engineController.SetPosition();
				//std::cout << "setting up starting position\n";
			}
			else {
				std::string fenString = arguments.substr(0, arguments.find("m"));
				engineController.SetPosition(fenString);
				//std::cout << "setting up fen: " << fenString << "\n";
			}
			if (instruction.find("moves")<instruction.length()) {
				std::string moveHistory = instruction.substr(instruction.find("moves ") + 1 + 5, instruction.length());
				engineController.MakeMoves(moveHistory);
				//std::cout << "parsing move history: " << moveHistory << "\n";
			}
		}
		if (key == "go") {
			std::string wTime;
			std::string bTime;
			if (instruction.length() != key.length()) {
				std::string options = instruction.substr(instruction.find(' ') + 1, instruction.length());
				if(options.find("wtime")<=options.length()){
					wTime = options.substr(options.find("wtime") + 6, options.length());
					wTime = wTime.substr(0, wTime.find(' '));
				}
				if (options.find("btime") <= options.length()) {
					bTime = options.substr(options.find("btime") + 6, options.length());
					bTime = bTime.substr(0, bTime.find(' '));
				}
			}
			//TO DO: parse options and dispatch engine search, once finished return here and
			std::string bestmove = "bestmove " + engineController.Search();
			std::cout << bestmove << "\n";
			//std::cout << "dispatching engine, with: wtime " << wTime << ", btime " << bTime << "\n";
		}
		if (key == "stop") {
			//TO DO: stop engine and return search results
			//std::cout << "stopping\n";
		}
		if (key == "quit") {
			return;
		}
		//Custom keys from here on
		if (key == "showboard") {
			std::cout << engineController.ShowBoard() << "\n";
		}
		if (key == "legalmoves") {
			std::cout << engineController.GetLegalMoves() << "\n";
		}
		if (key == "perft") {
			std::cout << engineController.Perft(std::stoi(instruction.substr(instruction.find(' ')+1,instruction.length()))-1) << "\n";
		}
		if (key == "splitperft") {
			std::cout << engineController.SplitPerft(std::stoi(instruction.substr(instruction.find(' ') + 1, instruction.length()))-1) << "\n";
		}
		if (key == "undolastmove") {
			engineController.UndoLastMove();
		}
	}
}