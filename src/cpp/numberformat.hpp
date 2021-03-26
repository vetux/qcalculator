#ifndef NUMBERFORMAT_HPP
#define NUMBERFORMAT_HPP

#include <string>
#include <sstream>
#include <climits>
#include <cmath>
#include <limits>
#include <bitset>
#include <iomanip>

#include "valuetype.hpp"

namespace NumberFormat {
    std::string toDecimal(ValueType v, int floatPrecision = 32);

    ValueType fromDecimal(const std::string &s);

    std::string toHex(ValueType v);

    ValueType fromHex(const std::string &s);

    std::string toOctal(ValueType v);

    ValueType fromOctal(const std::string &s);

    std::string toBinary(ValueType v);

    ValueType fromBinary(const std::string &s);
}

#endif // NUMBERFORMAT_HPP
