#pragma once
#include <chrono>

// Struct for time keeping during search
class Timer
{
    public:
    // Variables
    // --------------------------------------------------
    int wTime; // Remaining time for white in ms
    int bTime; // Remaining time for black in ms
    int winc;  // Increment for white
    int binc;  // Increment for black
    std::chrono::time_point<std::chrono::_V2::steady_clock> now;   // Starting time of search
    // --------------------------------------------------

    Timer(const int _wTime, const int _bTime);
    Timer(const int _wTime, const int _bTime, const int _winc, const int _binc);

    float TimeElapsed();
};