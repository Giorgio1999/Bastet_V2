#pragma once

namespace MathUtility
{
    template <typename T>
    class Random
    {
    private:
        T state;
    public:
        Random(T seed);
        T Next();
    };
}


template <typename T>
MathUtility::Random<T>::Random(T seed)
{
    state = seed;
}

template<typename T>
T MathUtility::Random<T>::Next(){
    // Constants by Lewis, Goodman and Miller
    const int a = 16807;
    const int c = 0;
    const int m = 2147483647;

    state = a*state+c%m;
    return state/m;
}