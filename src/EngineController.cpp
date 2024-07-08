#include "EngineController.h"
#include "Data.h"
#include "Engine.h"
#include "FenParser.h"
#include "Timer.h"
#include <array>
#include <chrono>
#include <iostream>
#include <string>
#include "TypeDefs.h"

EngineController::EngineController () {}

// UCI
// -------------------------------------------------------------------
bool
EngineController::BootEngine ()
{
    CERR << "EngineController: Booting Engine" << std::endl;
    CERR << "EngineController: Calling Engine()" << std::endl;
    engine = Engine ();
    startpos = Fen2Position ();
    CERR << "EngineController: Calling engine.Boot()" << std::endl;
    engine.Boot ();
    isReady = true;
    return true;
}

void
EngineController::SetStopFlag (const bool &value)
{
    engine.stopFlag = value;
}

void
EngineController::NewGame ()
{
    CERR << "EngineController: Receive Command newgame" << std::endl;
    if (!isReady)
        {
            BootEngine ();
        }
    CERR << "EngineController: Calling engine.NewGame()" << std::endl;
    engine.NewGame ();
}

void
EngineController::SetPosition ()
{
    CERR << "EngineController: Receive Command position startpos" << std::endl;
    if (!isReady)
        {
            BootEngine ();
        }
    CERR << "EngineController: Calling engine.SetBoard(startpos)" << std::endl;
    engine.SetBoard (startpos);
}

void
EngineController::SetPosition (const std::string &fenString)
{
    CERR << "EngineController: Receive Command position " << fenString << std::endl;
    if (!isReady)
        {
            BootEngine ();
        }
    CERR << "EngineController: Calling engine.SetBoard(" << fenString << ")" << std::endl;
    engine.SetBoard (Fen2Position (fenString));
}

void
EngineController::MakeMoves (std::string &moveHistory)
{
    CERR << "EngineController: Receive Command moves " << moveHistory << std::endl;
    std::vector<Mover> movers = Str2Moves (moveHistory);
    CERR << "EngineController: Calling engine.MakePermanentMove(" << moveHistory << ")" << std::endl;
    for (const auto &mover : movers)
        {
            engine.MakePermanentMove (Mover2Move (mover));
        }
}

void
EngineController::TestReady ()
{
    CERR << "EngineController: EngineController: Receive Command testready" << std::endl;
    isReady = true;
}

bool
EngineController::IsReady ()
{
    CERR << "EngineController: Receive Command isready" << std::endl;
    if (!isReady)
        {
            BootEngine ();
        }
    return isReady;
}

std::string
EngineController::Search (Timer timer)
{
    CERR << "EngineController: Receive Command go";
    CERR << " wtime " << timer.wTime;
    CERR << " btime " << timer.bTime;
    CERR << " winc " << timer.winc;
    CERR << " binc " << timer.binc;
    CERR << (timer.inf ? " infinite " : "");
    CERR << " searchmoves " << timer.searchmoveslist;
    CERR << " nodes " << timer.nodes;
    CERR << " depth " << timer.depth;
    CERR << " movetime " << timer.movetime;
    CERR << " movestogo " << timer.movestogo;
    CERR << std::endl;
    CERR << "EngineController: Calling engine.GetBestMove()" << std::endl;
    return Move2Str (engine.GetBestMove (timer));
}

std::string
EngineController::Search (const int wTime, const int bTime, const int winc, const int binc)
{
    CERR << "EngineController: Receive Command go wtime " << wTime << " btime " << bTime << " winc " << winc << " binc " << binc << std::endl;
    Timer timer (wTime, bTime, winc, binc);
    CERR << "EngineController: Calling engine.GetBestMove()" << std::endl;
    return Move2Str (engine.GetBestMove (timer));
}

std::string
EngineController::Options ()
{
    CERR << "EngineController: Printing options" << std::endl;
    std::string res = "option name Hash type spin default " + std::to_string (engine.ttSize) + " min 0 max 1024";
    return res;
}

void
EngineController::SetOptions (int _ttSize)
{
    CERR << "EngineController: Receive Command setoptions name Hash value " << _ttSize << std::endl;
    engine.ttSize = _ttSize;
    CERR << "EngineController: Calling transposition::Tt(" << _ttSize << ")" << std::endl;
    engine.tt = transposition::Tt (engine.ttSize);
}
// -------------------------------------------------------------------

// Non UCI
// -------------------------------------------------------------------
std::string
EngineController::Perft (int depth)
{
    bitboard numberOfLeafs = engine.Perft (depth);
    std::string returnString = std::to_string (numberOfLeafs);
    return returnString;
}

std::string
EngineController::SplitPerft (int depth)
{
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves (moveHolder, moveHolderIndex, false);
    std::string returnString = "";
    for (uint i = 0; i < moveHolderIndex; i++)
        {
            returnString += Move2Str (Move2Mover (moveHolder[i])) += ": ";
            engine.MakeMove (moveHolder[i]);
            bitboard res = 0;
            if (depth != 1)
                {
                    res = engine.Perft (depth - 1);
                }
            else
                {
                    res = 1;
                }
            returnString += std::to_string (res);
            engine.UndoLastMove ();
            returnString += "\n";
        }
    return returnString;
}

void
EngineController::FullPerftTest ()
{
    if (!isReady)
        {
            BootEngine ();
        }
    auto fullStart = std::chrono::high_resolution_clock::now ();
    for (uint i = 0; i < fullPerftSuite.size (); i++)
        {
            std::string line = fullPerftSuite.at (i);
            std::string fen = line.substr (0, line.find (','));
            std::cout << "Position: " << fen << ":" << std::endl;
            engine.SetBoard (Fen2Position (fen));
            line = line.substr (fen.size () + 1, line.size ());
            auto depth = 1;
            while (line.size () > 0 && !engine.stopFlag)
                {
                    std::cout << "\tdepth " << depth << ": ";
                    auto start = std::chrono::high_resolution_clock::now ();
                    bitboard result = engine.Perft (depth);
                    auto end = std::chrono::high_resolution_clock::now ();
                    float duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count ();
                    auto mnps = result / duration / 1000000. * 1000.;
                    std::string ref = line.substr (0, line.find_first_of (';'));
                    std::cout << result << "(" << ref << ")";
                    int diff = result - std::stoi (ref);
                    std::cout << " diff: " << diff << ", speed: " << mnps << "Mnps" << std::endl;
                    line = line.substr (ref.size () + 1, line.size ());
                    depth++;
                }
        }
    auto fullEnd = std::chrono::high_resolution_clock::now ();
    float duration = std::chrono::duration_cast<std::chrono::seconds> (fullEnd - fullStart).count ();
    std::cout << "Done! Total time: " << duration << "s" << std::endl;
}

void
EngineController::Bench ()
{
    if (!isReady)
        {
            BootEngine ();
        }
    auto fixedDepth = 4;
    auto fullStart = std::chrono::steady_clock::now ();
    bitboard nodesVisited = 0;
    for (uint i = 0; i < benchMarkingData.size (); i++)
        {
            std::string fen = benchMarkingData.at (i);
            engine.SetBoard (Fen2Position (fen));
            nodesVisited += engine.Perft (fixedDepth);
        }
    auto fullEnd = std::chrono::steady_clock::now ();
    float duration = std::chrono::duration_cast<std::chrono::milliseconds> (fullEnd - fullStart).count ();
    std::cout << std::to_string (nodesVisited) << " nodes " << std::to_string ((int)(nodesVisited / duration * 1000)) << " nps" << std::endl;
}

void
EngineController::Validate ()
{
    if (!isReady)
        {
            BootEngine ();
        }
    auto start = std::chrono::steady_clock::now ();
    auto valid = true;
    for (uint i = 0; i < fullPerftSuite.size (); i++)
        {
            std::string line = fullPerftSuite.at (i);
            std::string fen = line.substr (0, line.find (','));
            engine.SetBoard (Fen2Position (fen));
            line = line.substr (fen.size () + 1, line.size ());
            auto depth = 1;
            while (line.size () > 0 && !engine.stopFlag)
                {
                    auto result = engine.Perft (depth);
                    std::string ref = line.substr (0, line.find_first_of (';'));
                    auto diff = result - std::stoi (ref);
                    line = line.substr (ref.size () + 1, line.size ());
                    if (diff != 0)
                        {
                            valid = false;
                            break;
                        }
                    depth++;
                }
        }
    auto end = std::chrono::steady_clock::now ();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count () / 1000;
    std::cout << (valid ? "Is valid, " : "Is not valid, ") << std::to_string (duration) << "s" << std::endl;
}

std::string
EngineController::Evaluate ()
{
    if (!isReady)
        {
            BootEngine ();
        }
    std::string result = std::to_string (engine.Evaluate ());
    return result;
}

void
EngineController::HashTest (int depth)
{
    if (!isReady)
        {
            BootEngine ();
        }
    SetPosition ();
    engine.HashTest (depth);
}

void
EngineController::HashTest ()
{
    if (!isReady)
        {
            BootEngine ();
        }
    for (uint i = 0; i < fullPerftSuite.size (); i++)
        {
            std::string line = fullPerftSuite.at (i);
            std::string fen = line.substr (0, line.find (','));
            std::cout << "Position: " << fen << std::endl;
            engine.SetBoard (Fen2Position (fen));
            line = line.substr (fen.size () + 1, line.size ());
            int depth = 1;
            while (line.size () > 0 && !engine.stopFlag)
                {
                    engine.HashTest (depth);
                    std::string ref = line.substr (0, line.find_first_of (';'));
                    line = line.substr (ref.size () + 1, line.size ());
                    std::cout << "\t depth: " << depth << " done" << std::endl;
                    depth++;
                }
            std::cout << "Done" << std::endl;
        }
}
// -------------------------------------------------------------------

// Debugging
// -------------------------------------------------------------------
std::string
EngineController::GetLegalMoves (bool capturesOnly)
{
    std::array<move, 256> moveHolder;
    uint moveHolderIndex = 0;
    engine.GetLegalMoves (moveHolder, moveHolderIndex, capturesOnly);
    std::string movesString = "";
    for (uint i = 0; i < moveHolderIndex; i++)
        {
            movesString += Move2Str (Move2Mover (moveHolder[i])) + " ";
        }
    return movesString;
}

std::string
EngineController::ShowBoard ()
{
    return engine.ShowBoard ();
}

void
EngineController::UndoLastMove ()
{
    engine.UndoLastMove ();
}
// -------------------------------------------------------------------
