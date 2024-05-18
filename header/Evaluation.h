#pragma once
#include "Engine.h"

const int pieceValues[5] = {1, 3, 3, 5, 9};

namespace Evaluation
{
    int StaticEvaluation(Engine &engine);
}
