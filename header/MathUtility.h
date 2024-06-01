#pragma once
#include <array>
#include <iostream>

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
        T Next(const T leftBound, const T rightBound);
    };

    template <typename T, typename R, int SIZE>
    void Sort(std::array<T, SIZE> &target, std::array<R, SIZE> &comparer, int upTo, bool descending);
}

template <typename T>
MathUtility::Random<T>::Random(T seed)
{
    state = seed;
}

template <typename T>
T MathUtility::Random<T>::Next()
{
    // Constants by Lewis, Goodman and Miller
    const uint32_t a = 16807;
    const uint32_t c = 0;
    const uint32_t m = 2147483647;

    // state = a*state+c%m;
    state = (a * state + c) % m;
    // return state/m;
    return state;
}

template <typename T>
T MathUtility::Random<T>::Next(const T leftBound, const T rightBound)
{
    double prng = (double)Next();
    prng /= (double)2147483647;
    double m = rightBound - leftBound;
    double b = leftBound;
    return (T)(prng * m + b);
}

template <typename T, typename R, int SIZE>
void MathUtility::Sort(std::array<T, SIZE> &target, std::array<R, SIZE> &comparer, int upTo, bool descending)
{
    T tmpT;
    R tmpR;
    if (descending)
    {
        for (auto n = 1; n < upTo; n++)
        {
            for (auto i = 0; i < upTo - 1; i++)
            {
                if (comparer[i] < comparer[i + 1])
                {
                    tmpT = target[i];
                    target[i] = target[i + 1];
                    target[i + 1] = tmpT;
                    tmpR = comparer[i];
                    comparer[i] = comparer[i + 1];
                    comparer[i + 1] = tmpR;
                }
            }
        }
    }
    else
    {
        for (auto n = 1; n < upTo; n++)
        {
            for (auto i = 0; i < upTo - 1; i++)
            {
                if (comparer[i] > comparer[i + 1])
                {
                    tmpT = target[i];
                    target[i] = target[i + 1];
                    target[i + 1] = tmpT;
                    tmpR = comparer[i];
                    comparer[i] = comparer[i + 1];
                    comparer[i + 1] = tmpR;
                }
            }
        }
    }
}