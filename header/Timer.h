#pragma once

// Struct for time keeping during search
struct Timer
{
    // Variables
    // --------------------------------------------------
    int wTime;  // Remaining time for white in ms
    int bTime;  // Remaining time for black in ms
    // --------------------------------------------------

    Timer(const int &wTime, const int &bTime);
};