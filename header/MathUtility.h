#pragma once
#include <array>
#include <iostream>

namespace MathUtility
{
template <typename T> class Random
{
  private:
    T state;

  public:
    Random (T seed);
    T Next ();
    T Next (const T leftBound, const T rightBound);
};

template <typename T, typename R, int SIZE> void Sort (std::array<T, SIZE> &target, std::array<R, SIZE> &comparer, int upTo, bool descending);

template <typename T1, typename T2, typename R, int size> void Sort (std::array<T1, size> &target1, std::array<T2, size> &target2, std::array<R, size> &comparer, int upTo, bool descending);
} // namespace MathUtility

template <typename T> MathUtility::Random<T>::Random (T seed) { state = seed; }

template <typename T>
T
MathUtility::Random<T>::Next ()
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
T
MathUtility::Random<T>::Next (const T leftBound, const T rightBound)
{
    double prng = (double)Next ();
    prng /= (double)2147483647;
    double m = rightBound - leftBound;
    double b = leftBound;
    return (T)(prng * m + b);
}

template <typename T, typename R, int SIZE>
void
MathUtility::Sort (std::array<T, SIZE> &target, std::array<R, SIZE> &comparer, int upTo, bool descending)
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

template <typename T1, typename T2, typename R, int size>
void
MathUtility::Sort (std::array<T1, size> &target1, std::array<T2, size> &target2, std::array<R, size> &comparer, int upTo, bool descending)
{
    T1 tmpT1;
    T2 tmpT2;
    R tmpR;
    if (descending)
        {
            for (auto n = 1; n < upTo; n++)
                {
                    for (auto i = 0; i < upTo - 1; i++)
                        {
                            if (comparer[i] < comparer[i + 1])
                                {
                                    tmpT1 = target1[i];
                                    target1[i] = target1[i + 1];
                                    target1[i + 1] = tmpT1;
                                    tmpT2 = target2[i];
                                    target2[i] = target2[i + 1];
                                    target2[i + 1] = tmpT2;
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
                                    tmpT1 = target1[i];
                                    target1[i] = target1[i + 1];
                                    target1[i + 1] = tmpT1;
                                    tmpT2 = target2[i];
                                    target2[i] = target2[i + 1];
                                    target2[i + 1] = tmpT2;
                                    tmpR = comparer[i];
                                    comparer[i] = comparer[i + 1];
                                    comparer[i + 1] = tmpR;
                                }
                        }
                }
        }
}

