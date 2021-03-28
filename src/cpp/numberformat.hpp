#ifndef NUMBERFORMAT_HPP
#define NUMBERFORMAT_HPP

#include <string>
#include <sstream>
#include <climits>
#include <cmath>
#include <limits>
#include <bitset>
#include <iomanip>

#include "calc/arithmetictype.hpp"

namespace NumberFormat {
    std::string toDecimal(ArithmeticType v, int floatPrecision = 32);

    std::string toHex(ArithmeticType v);

    std::string toOctal(ArithmeticType v);

    std::string toBinary(ArithmeticType v);

    ArithmeticType fromDecimal(const std::string &s);

    ArithmeticType fromHex(const std::string &s);

    ArithmeticType fromOctal(const std::string &s);

    ArithmeticType fromBinary(const std::string &s);
}

#endif // NUMBERFORMAT_HPP
