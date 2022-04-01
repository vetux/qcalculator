/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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
