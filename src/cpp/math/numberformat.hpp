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
    std::string toDecimal(const ArithmeticType &v, int decimalSpaces);

    std::string toHex(const ArithmeticType &v, int decimalSpaces);

    std::string toOctal(const ArithmeticType &v, int decimalSpaces);

    std::string toBinary(const ArithmeticType &v, int decimalSpaces);

    ArithmeticType fromDecimal(const std::string &s, int decimalSpaces);

    ArithmeticType fromHex(const std::string &s, int decimalSpaces);

    ArithmeticType fromOctal(const std::string &s, int decimalSpaces);

    ArithmeticType fromBinary(const std::string &s, int decimalSpaces);
}

#endif // NUMBERFORMAT_HPP
