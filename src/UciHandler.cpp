#include "UciHandler.h"
#include "EngineController.h"
#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <future>

void Listen()
{
	EngineController engineController;
	std::string instruction = "";
	while (std::getline(std::cin, instruction))
	{
		std::string key = instruction.substr(0, instruction.find(' '));
		if (instruction == "")
		{
			continue;
		}
		if (key == "uci")
		{
			std::cout << "id name Bastet\n";
			std::cout << "id author G. Lovato\n";
			std::cout << "uciok\n";
			engineController.BootEngine();
		}
		if (key == "isready")
		{
			while (!engineController.IsReady())
			{
				engineController.TestReady();
			}
			std::cout << "readyok\n";
		}
		if (key == "ucinewgame")
		{
			engineController.NewGame();
		}
		if (key == "position" && instruction.length() != key.length())
		{
			std::string arguments = instruction.substr(key.length() + 1, instruction.length());
			std::string positionKey = arguments.substr(0, arguments.find(' '));
			if (positionKey == "startpos")
			{
				engineController.SetPosition();
			}
			else
			{
				std::string fenString = arguments.substr(0, arguments.find("m"));
				engineController.SetPosition(fenString);
			}
			if (instruction.find("moves") < instruction.length())
			{
				std::string moveHistory = instruction.substr(instruction.find("moves ") + 1 + 5, instruction.length());
				engineController.MakeMoves(moveHistory);
			}
		}
		if (key == "go")
		{
			std::string wTime;
			std::string bTime;
			if (instruction.length() != key.length())
			{
				std::string options = instruction.substr(instruction.find(' ') + 1, instruction.length());
				if (options.find("wtime") <= options.length())
				{
					wTime = options.substr(options.find("wtime") + 6, options.length());
					wTime = wTime.substr(0, wTime.find(' '));
				}
				if (options.find("btime") <= options.length())
				{
					bTime = options.substr(options.find("btime") + 6, options.length());
					bTime = bTime.substr(0, bTime.find(' '));
				}
			}
			// TO DO: parse options and dispatch engine search, once finished return here and
			std::thread([&engineController]
						{std::string result =  "bestmove " + engineController.Search();
						std::cout << result << std::endl;
						engineController.SetStopFlag(false); })
				.detach();
			// std::string result = "bestmove " + engineController.Search();
			// std::cout << result << "\n";
		}
		if (key == "stop")
		{
			engineController.SetStopFlag(true);
			continue;
		}
		if (key == "quit")
		{
			return;
		}
		// Custom keys from here on
		if (key == "showboard")
		{
			std::cout << engineController.ShowBoard();
		}
		if (key == "legalmoves")
		{
			std::cout << engineController.GetLegalMoves();
		}
		if (key == "perft")
		{
			auto depth = std::stoi(instruction.substr(instruction.find(' ') + 1, instruction.length()));
			std::thread([&engineController, depth]
						{
				std::string result = engineController.Perft(depth);
				std::cout << result << "\n";
				engineController.SetStopFlag(false); })
				.detach();
		}
		if (key == "splitperft")
		{
			auto depth = std::stoi(instruction.substr(instruction.find(' ') + 1, instruction.length()));
			std::thread([&engineController, &depth]
						{
				std::string result = engineController.SplitPerft(depth);
				std::cout << result << "\n"; 
				engineController.SetStopFlag(false); })
				.detach();
		}
		if (key == "fullperft")
		{
			std::thread([&engineController]
						{
				engineController.FullPerftTest();
				engineController.SetStopFlag(false); })
				.detach();
		}
		if (key == "undolastmove")
		{
			engineController.UndoLastMove();
		}
	}
}