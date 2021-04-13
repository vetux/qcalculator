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

#include "math/numberformat.hpp"

#include "math/fractiontest.hpp"

using namespace FractionTest;

std::string getRoundingFormatChar(mpfr_rnd_t mode) {
    switch (mode) {
        default:
        case MPFR_RNDN:
            return "N";
        case MPFR_RNDZ:
            return "Z";
        case MPFR_RNDU:
            return "U";
        case MPFR_RNDD:
            return "D";
        case MPFR_RNDA:
            return "Y";
    }
}

std::string NumberFormat::toDecimal(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding) {
    std::string ret = v.toString("%."
                                 + std::to_string(decimalSpaces)
                                 + "R" + getRoundingFormatChar(rounding)
                                 + "f");
    if (ret.empty() || ret.find('.') == std::string::npos) {
        return ret;
    } else {
        std::string zeroStrip = ret;
        for (auto it = ret.rbegin(); it != ret.rend(); it++) {
            if (*it == '.') {
                zeroStrip.pop_back();
                break;
            } else if (*it != '0') {
                break;
            } else {
                zeroStrip.pop_back();
            }
        }
        return zeroStrip;
    }
}

std::string NumberFormat::toHex(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding) {
    return v.toString("%."
                      + std::to_string(decimalSpaces)
                      + "R" + getRoundingFormatChar(rounding)
                      + "a");
}

std::string NumberFormat::toOctal(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding) {
    if (v < 0) {
        throw std::runtime_error("Cannot convert negative number to octal");
    } else if (hasFraction(v)) {
        throw std::runtime_error("Cannot convert number with fraction to octal");
    } else if (v > mpfr::mpreal(std::numeric_limits<long long>::max())) {
        throw std::runtime_error("Cannot convert number to octal (Out of range)");
    } else {
        long long val = v.toLLong();
        std::stringstream stream;
        stream << std::oct << val;
        return stream.str();
    }
}

std::string NumberFormat::toBinary(const ArithmeticType &v, int decimalSpaces, mpfr_rnd_t rounding) {
    if (v < 0) {
        throw std::runtime_error("Cannot convert negative number to binary");
    } else if (hasFraction(v)) {
        throw std::runtime_error("Cannot convert number with fraction to binary");
    } else if (v > mpfr::mpreal(std::numeric_limits<long long>::max())) {
        throw std::runtime_error("Cannot convert number to binary (Out of range)");
    } else {
        std::string ret;
        std::string bitstring = std::bitset<sizeof(long long) * CHAR_BIT>(v.toLLong()).to_string();
        for (size_t i = 0; i < bitstring.size(); i++) {
            if (bitstring.at(i) != '0') {
                return bitstring.substr(i);
            }
        }
        return "0";
    }
}

ArithmeticType NumberFormat::fromDecimal(const std::string &s, int precision, mpfr_rnd_t rounding) {
    return mpfr::mpreal(s, precision, 10, rounding);
}

ArithmeticType NumberFormat::fromHex(const std::string &s, int precision, mpfr_rnd_t rounding) {
    return mpfr::mpreal(s, precision, 16, rounding);
}

ArithmeticType NumberFormat::fromOctal(const std::string &s, int precision, mpfr_rnd_t rounding) {
    return mpfr::mpreal(s, precision, 8, rounding);
}

ArithmeticType NumberFormat::fromBinary(const std::string &s, int precision, mpfr_rnd_t rounding) {
    if (s.size() > 63)
        throw std::runtime_error("Maximum 64 bits can be converted from string");
    return mpfr::mpreal(std::bitset<64>(s).to_ullong(), precision, rounding);
}