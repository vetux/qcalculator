#include "fractiontest.hpp"

bool FractionTest::hasFraction(long double v) {
    long double intPart = 0;
    return std::modf(v, &intPart) != 0;
}