#include "fractiontest.hpp"

bool FractionTest::hasFraction(long double v) {
    long double intPart = 0;
    return std::modf(v, &intPart) != 0;
}

bool FractionTest::hasFraction(const mpfr::mpreal &v) {
    mpfr::mpreal i;
    mpfr::mpreal f;
    mpfr_modf(i.mpfr_ptr(), f.mpfr_ptr(), v.mpfr_ptr(), MPFR_RNDN);
    return f != 0;
}
