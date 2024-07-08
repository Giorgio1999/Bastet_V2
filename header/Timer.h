#pragma once
#include "BitBoardUtility.h"
#include "BoardUtility.h"
#include <string>
#include <vector>
#include <chrono>

// Struct for time keeping during search
class Timer
{
  public:
    // Variables
    // --------------------------------------------------
    int wTime = 0;                                               // Remaining time for white in ms
    int bTime = 0;                                               // Remaining time for black in ms
    int winc = 0;                                                // Increment for white
    int binc = 0;                                                // Increment for black
    std::chrono::time_point<std::chrono::_V2::steady_clock> now; // Starting time of search
    bool inf = false;                                            // If true search until stop
    int depth = INT32_MAX;                                       // allowed depth in plies
    bitboard nodes = UINT64_MAX;                                 // allowed number of nodes to be searched
    int movestogo = INT32_MAX;                                   // moves until next time control
    std::string searchmoveslist = "";                            // string of searchmoves
    std::vector<Mover> searchmoves;                              // List of moves to restrict search to
    bool moverestriction = false;                                // flag wheter searchmoves specified
    int movetime = INT32_MAX;                                    // flat time of move
    // --------------------------------------------------

    Timer ();
    Timer (const int _wTime, const int _bTime);
    Timer (const int _wTime, const int _bTime, const int _winc, const int _binc);
    Timer (const bool _inf);
    void SetSearchmoves ();
    float TimeElapsed ();
};
