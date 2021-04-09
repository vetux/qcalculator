#ifndef COMMON_HPP
#define COMMON_HPP

#include <cmath>

#include "extern/mpreal.h"

namespace FractionTest {
    bool hasFraction(long double v);

    bool hasFraction(const mpfr::mpreal &v);
}

#endif // COMMON_HPP
