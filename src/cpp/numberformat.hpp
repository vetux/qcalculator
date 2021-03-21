#ifndef NUMERALREP_HPP
#define NUMERALREP_HPP

#include <string>
#include <sstream>
#include <climits>
#include <cmath>
#include <limits>
#include <bitset>
#include <iomanip>

#include "fractiontest.hpp"
#include "valuetype.hpp"

class NumberFormat {
public:
    static std::string toDecimal(ValueType v, int floatPrecision = 32);

    static ValueType fromDecimal(const std::string &s);

    static std::string toHex(ValueType v);

    static ValueType fromHex(const std::string &s);

    static std::string toOctal(ValueType v);

    static ValueType fromOctal(const std::string &s);

    static std::string toBinary(ValueType v);

    static ValueType fromBinary(const std::string &s);
};

#endif // NUMERALREP_HPP
