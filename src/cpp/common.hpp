#ifndef COMMON_HPP
#define COMMON_HPP

#include <cmath>

bool hasFraction(long double v)
{
    long double intPart = 0;
    return std::modf(v, &intPart) != 0;
}

#endif // COMMON_HPP
