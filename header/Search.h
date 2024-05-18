#pragma once
#include "Timer.h"
#include "BitBoardUtility.h"
#include "Engine.h"

namespace Search
{
    move GetBestMove(Engine &engine, const Timer &timer);
}

int FixedDepthSearch(Engine&engine, int depth);