#include "UciHandler.h"
#include "EngineController.h"
#include <chrono>
#include <iostream>
#include <regex>
#include <string>
#include <thread>

void
Listen ()
{
    EngineController engineController;
    std::string instruction = "";
    std::regex format ("\\S+");
    std::smatch match;
    while (std::getline (std::cin, instruction))
        {
            std::regex_iterator<std::string::iterator> rit (instruction.begin (), instruction.end (), format);
            std::regex_iterator<std::string::iterator> rend;
            if (rit == rend)
                {
                    continue;
                }
            std::string key = rit->str ();
            if (key == "uci")
                {
                    engineController.BootEngine ();
                    std::cout << "id name Bastet" << std::endl;
                    std::cout << "id author G. Lovato" << std::endl;
                    std::cout << engineController.Options () << std::endl;
                    std::cout << "uciok" << std::endl;
                    continue;
                }
            if (key == "setoption")
                {
                    rit++;
                    rit++;
                    std::string name = rit->str ();
                    if (name == "Hash")
                        {
                            rit++;
                            rit++;
                            engineController.SetOptions (std::stoi (rit->str ()));
                        }
                }
            if (key == "isready")
                {
                    if (engineController.IsReady ())
                        {
                            std::cout << "readyok" << std::endl;
                        }
                    continue;
                }
            if (key == "ucinewgame")
                {
                    engineController.NewGame ();
                    continue;
                }
            if (key == "position")
                {
                    rit++;
                    std::string arg1 = rit->str ();
                    if (arg1 == "startpos")
                        {
                            engineController.SetPosition ();
                        }
                    else if (arg1 == "fen")
                        {
                            rit++;
                            std::string fen = rit->str () + " "; // pieces
                            rit++;
                            fen += rit->str () + " "; // Color to move
                            rit++;
                            fen += rit->str () + " "; // Castling
                            rit++;
                            fen += rit->str () + " "; // enpassant
                            rit++;
                            fen += rit->str () + " "; // ?
                            rit++;
                            fen += rit->str () + " "; // ?
                            engineController.SetPosition (fen);
                        }
                    else if (arg1 != "")
                        {
                            std::string fen = rit->str () + " "; // pieces
                            rit++;
                            fen += rit->str () + " "; // Color to move
                            rit++;
                            fen += rit->str () + " "; // Castling
                            rit++;
                            fen += rit->str () + " "; // enpassant
                            rit++;
                            fen += rit->str () + " "; // Number of nonreversible moves
                            rit++;
                            fen += rit->str () + " "; // Move number
                            engineController.SetPosition (fen);
                        }
                    rit++;
                    std::string arg2 = rit->str ();
                    if (arg2 == "moves")
                        {
                            rit++;
                            std::string moveHistory = "";
                            for (auto i = rit; i != rend; i++)
                                {
                                    moveHistory += i->str () + " ";
                                }
                            engineController.MakeMoves (moveHistory);
                        }
                    continue;
                }
            if (key == "go")
                {
                    Timer timer;
                    rit++;
                    bool parsingMoves = false;
                    while (rit != rend)
                        {
                            std::string arg = rit->str ();
                            CERR << arg << std::endl;
                            if (arg == "searchmoves")
                                {
                                    if (!parsingMoves)
                                        {
                                            timer.moverestriction = true;
                                            parsingMoves = true;
                                            rit++;
                                            arg = rit->str ();
                                        }
                                }
                            if (arg == "ponder")
                                {
                                    parsingMoves = false;
                                    CERR << arg << " not supported" << std::endl;
                                }
                            if (arg == "wtime")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    timer.wTime = std::stoi (rit->str ());
                                }
                            if (arg == "btime")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    timer.bTime = std::stoi (rit->str ());
                                }
                            if (arg == "winc")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    timer.winc = std::stoi (rit->str ());
                                }
                            if (arg == "binc")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    timer.binc = std::stoi (rit->str ());
                                }
                            if (arg == "movestogo")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    timer.movestogo = std::stoi (rit->str ());
                                }
                            if (arg == "depth")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    timer.depth = std::stoi (rit->str ());
                                }
                            if (arg == "nodes")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    timer.nodes = std::stoul (rit->str ());
                                }
                            if (arg == "mate")
                                {
                                    parsingMoves = false;
                                    CERR << arg << " not supported" << std::endl;
                                }
                            if (arg == "movetime")
                                {
                                    parsingMoves = false;
                                    rit++;
                                    int movetime = std::stoi (rit->str ());
                                    std::thread ([&engineController, movetime] {
                                        std::this_thread::sleep_for (std::chrono::milliseconds (movetime));
                                        engineController.SetStopFlag (true);
                                    }).detach ();
                                }
                            if (arg == "infinite")
                                {
                                    parsingMoves = false;
                                    timer.inf = true;
                                }
                            if (parsingMoves)
                                {
                                    timer.searchmoveslist += arg + " ";
                                }
                            rit++;
                        }
                    timer.SetSearchmoves ();
                    std::thread ([&engineController, timer] {
                        std::string result = "bestmove " + engineController.Search (timer);
                        std::cout << result << std::endl;
                        engineController.SetStopFlag (false);
                    }).detach ();
                    continue;
                }
            if (key == "stop")
                {
                    engineController.SetStopFlag (true);
                    continue;
                }
            if (key == "quit")
                {
                    return;
                }
            if (key == "showboard")
                {
                    std::cout << engineController.ShowBoard ();
                    continue;
                }
            if (key == "legalmoves")
                {
                    rit++;
                    bool capturesOnly = rit->str () == "captures";
                    std::cout << engineController.GetLegalMoves (capturesOnly) << std::endl;
                    continue;
                }
            if (key == "perft")
                {
                    rit++;
                    auto depth = std::stoi (rit->str ());
                    rit++;
                    std::string arg = rit->str ();
                    if (arg != "")
                        {
                            std::string fen = rit->str () + " "; // pieces
                            rit++;
                            fen += rit->str () + " "; // Color to move
                            rit++;
                            fen += rit->str () + " "; // Castling
                            rit++;
                            fen += rit->str () + " "; // enpassant
                            rit++;
                            fen += rit->str () + " "; // ?
                            rit++;
                            fen += rit->str () + " "; // ?
                            engineController.SetPosition (fen);
                        }
                    std::thread ([&engineController, depth] {
                        std::string result = engineController.Perft (depth);
                        std::cout << result << std::endl;
                        engineController.SetStopFlag (false);
                    }).detach ();
                    continue;
                }
            if (key == "splitperft")
                {
                    rit++;
                    auto depth = std::stoi (rit->str ());
                    std::thread ([&engineController, depth] {
                        std::string result = engineController.SplitPerft (depth);
                        std::cout << result << std::endl;
                        engineController.SetStopFlag (false);
                    }).detach ();
                    continue;
                }
            if (key == "evaluate")
                {
                    std::cout << engineController.Evaluate () << std::endl;
                    continue;
                }
        }
}
