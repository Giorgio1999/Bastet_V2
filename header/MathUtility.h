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
        T Next(const T leftBound,const T rightBound);
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
    const uint32_t a = 16807;
    const uint32_t c = 0;
    const uint32_t m = 2147483647;

    // state = a*state+c%m;
    state = (a*state+c)%m;
    // return state/m;
    return state;
}

template<typename T>
T MathUtility::Random<T>::Next(const T leftBound,const T rightBound){
    double prng = (double)Next();
    prng/= (double)2147483647;
    double m = rightBound-leftBound;
    double b = leftBound;
    return (T)(prng*m+b);
}