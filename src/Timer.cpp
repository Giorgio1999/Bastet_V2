#include <Timer.h>

Timer::Timer(const int _wTime, const int _bTimer)
{
    wTime = _wTime;
    bTime = _bTimer;
    now = std::chrono::steady_clock::now();
}

Timer::Timer(const int _wTime, const int _bTimer, const int _winc, const int _binc)
{
    wTime = _wTime;
    bTime = _bTimer;
    winc = _winc;
    binc = _binc;
    now = std::chrono::steady_clock::now();
}

float Timer::TimeElapsed(){
    auto current = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current-now).count();
    return elapsed;
}