#include "UciHandler.h"
#include "EngineController.h"
#include <iostream>
#include <string>
#include <thread>
#include <regex>

void Listen()
{
	EngineController engineController;
	std::string instruction = "";
	std::regex format("\\S+");
	std::smatch match;
	while (std::getline(std::cin, instruction))
	{
		std::regex_iterator<std::string::iterator> rit(instruction.begin(), instruction.end(), format);
		std::regex_iterator<std::string::iterator> rend;
		// for(auto i=rit;i!=rend;i++){
		// 	std::cout << i->str() << " ";
		// }
		// std::cout << std::endl;
		if (rit == rend)
		{
			continue;
		}
		std::string key = rit->str();
		if (key == "uci")
		{
			engineController.BootEngine();
			std::cout << "id name Bastet\n";
			std::cout << "id author G. Lovato\n";
			std::cout << "uciok\n";
			continue;
		}
		if (key == "isready")
		{
			if (engineController.IsReady())
			{
				std::cout << "readyok\n";
			}
			continue;
		}
		if (key == "ucinewgame")
		{
			engineController.NewGame();
			continue;
		}
		if (key == "position")
		{
			rit++;
			std::string arg1 = rit->str();
			if (arg1 == "startpos")
			{
				engineController.SetPosition();
			}
			else if (arg1 == "fen")
			{
				rit++;
				std::string fen = rit->str() + " "; // pieces
				rit++;
				fen += rit->str() + " "; // Color to move
				rit++;
				fen += rit->str() + " "; // Castling
				rit++;
				fen += rit->str() + " "; // enpassant
				rit++;
				fen += rit->str() + " "; // ?
				rit++;
				fen += rit->str() + " "; // ?
				engineController.SetPosition(fen);
			}
			else if (arg1 != "")
			{
				std::string fen = rit->str() + " "; // pieces
				rit++;
				fen += rit->str() + " "; // Color to move
				rit++;
				fen += rit->str() + " "; // Castling
				rit++;
				fen += rit->str() + " "; // enpassant
				rit++;
				fen += rit->str() + " "; // Number of nonreversible moves
				rit++;
				fen += rit->str() + " "; // Move number
				engineController.SetPosition(fen);
			}
			rit++;
			std::string arg2 = rit->str();
			if (arg2 == "moves")
			{
				rit++;
				std::string moveHistory = "";
				for (auto i = rit; i != rend; i++)
				{
					moveHistory += i->str() + " ";
				}
				engineController.MakeMoves(moveHistory);
			}
			continue;
		}
		if (key == "go")
		{
			rit++;
			// std::string searchmoves = "";
			// auto ponder = false;
			auto wtime = 0;
			auto btime = 0;
			auto winc = 0;
			auto binc = 0;
			// auto movestogo = 0;
			// auto depth = 0;
			// auto nodes = 0;
			// auto mate = 0;
			// auto movetime = 0;
			// auto infinite = false;
			while (rit != rend)
			{
				std::string arg = rit->str();
				if (arg == "searchmoves")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				if (arg == "ponder")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				if (arg == "wtime")
				{
					rit++;
					wtime = std::stoi(rit->str());
				}
				if (arg == "btime")
				{
					rit++;
					btime = std::stoi(rit->str());
				}
				if (arg == "winc")
				{
					rit++;
					winc = std::stoi(rit->str());
				}
				if (arg == "binc")
				{
					rit++;
					binc = std::stoi(rit->str());
				}
				if (arg == "movestogo")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				if (arg == "depth")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				if (arg == "nodes")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				if (arg == "mate")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				if (arg == "movetime")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				if (arg == "infinite")
				{
					std::cout << arg << " not supported" << std::endl;
				}
				rit++;
			}
			std::thread([&engineController, wtime, btime, winc, binc]
						{std::string result =  "bestmove " + engineController.Search(wtime,btime,winc,binc);
									std::cout << result << std::endl;
									engineController.SetStopFlag(false); })
				.detach();
			continue;
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
		if (key == "showboard")
		{
			std::cout << engineController.ShowBoard();
			continue;
		}
		if (key == "legalmoves")
		{
			rit++;
			bool capturesOnly = rit->str() == "captures";
			std::cout << engineController.GetLegalMoves(capturesOnly) << std::endl;
			continue;
		}
		if (key == "perft")
		{
			rit++;
			auto depth = std::stoi(rit->str());
			rit++;
			std::string arg = rit->str();
			if (arg != "")
			{
				std::string fen = rit->str() + " "; // pieces
				rit++;
				fen += rit->str() + " "; // Color to move
				rit++;
				fen += rit->str() + " "; // Castling
				rit++;
				fen += rit->str() + " "; // enpassant
				rit++;
				fen += rit->str() + " "; // ?
				rit++;
				fen += rit->str() + " "; // ?
				engineController.SetPosition(fen);
			}
			std::thread([&engineController, depth]
						{
				std::string result = engineController.Perft(depth);
				std::cout << result << std::endl;
				engineController.SetStopFlag(false); })
				.detach();
			continue;
		}
		if (key == "splitperft")
		{
			rit++;
			auto depth = std::stoi(rit->str());
			std::thread([&engineController, depth]
						{
				std::string result = engineController.SplitPerft(depth);
				std::cout << result << std::endl;
				engineController.SetStopFlag(false); })
				.detach();
			continue;
		}
	}
}
