#ifndef CONSTEXPR_H
#define CONSTEXPR_H

#include <QtGlobal>

constexpr int countConsecutiveSetBit(uint value)
{
    int count = 0;
    while(value > 0)
    {
        value >>= 1;
        count++;
    }

    return count;
}

#endif // CONSTEXPR_H
