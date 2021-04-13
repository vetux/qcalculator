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

#ifndef NUMBERFORMAT_HPP
#define NUMBERFORMAT_HPP

#include <string>
#include <sstream>
#include <climits>
#include <cmath>
#include <limits>
#include <bitset>
#include <iomanip>

#include "math/arithmetictype.hpp"

namespace NumberFormat {
    std::string toDecimal(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding);

    std::string toHex(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding);

    std::string toOctal(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding);

    std::string toBinary(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding);

    ArithmeticType fromDecimal(const std::string &s, int precision, mpfr_rnd_t rounding);

    ArithmeticType fromHex(const std::string &s, int precision, mpfr_rnd_t rounding);

    ArithmeticType fromOctal(const std::string &s, int precision, mpfr_rnd_t rounding);

    ArithmeticType fromBinary(const std::string &s, int precision, mpfr_rnd_t rounding);
}

#endif // NUMBERFORMAT_HPP
