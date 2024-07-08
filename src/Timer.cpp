#include "BoardUtility.h"
#include <string>
#include <vector>
#include <Timer.h>

Timer::Timer () { now = std::chrono::steady_clock::now (); }

Timer::Timer (const int _wTime, const int _bTimer)
{
    wTime = _wTime;
    bTime = _bTimer;
    now = std::chrono::steady_clock::now ();
}

Timer::Timer (const int _wTime, const int _bTimer, const int _winc, const int _binc)
{
    wTime = _wTime;
    bTime = _bTimer;
    winc = _winc;
    binc = _binc;
    now = std::chrono::steady_clock::now ();
}

Timer::Timer (const bool _inf)
{
    inf = _inf;
    now = std::chrono::steady_clock::now ();
}

void
Timer::SetSearchmoves ()
{
    if (searchmoveslist != "")
        {
            searchmoveslist = searchmoveslist.substr (0, searchmoveslist.size () - 1);
            searchmoves = Str2Moves (searchmoveslist);
        }
}

float
Timer::TimeElapsed ()
{
    auto current = std::chrono::steady_clock::now ();
    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (current - now).count ();
    return elapsed;
}
