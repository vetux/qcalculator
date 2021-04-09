#include "math/numberformat.hpp"

#include "math/fractiontest.hpp"

using namespace FractionTest;

std::string NumberFormat::toDecimal(const ArithmeticType &v, int decimalSpaces) {
    return v.toString("%." + std::to_string(decimalSpaces) + "Rf");
}

std::string NumberFormat::toHex(const ArithmeticType &v, int decimalSpaces) {
    return v.toString("%." + std::to_string(decimalSpaces) + "Ra");
}

std::string NumberFormat::toOctal(const ArithmeticType &v, int decimalSpaces) {
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

std::string NumberFormat::toBinary(const ArithmeticType &v, int decimalSpaces) {
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

ArithmeticType NumberFormat::fromDecimal(const std::string &s, int decimalSpaces) {
    return mpfr::mpreal(s, mpfr::digits2bits(decimalSpaces));
}

ArithmeticType NumberFormat::fromHex(const std::string &s, int decimalSpaces) {
    return mpfr::mpreal(s, mpfr::digits2bits(decimalSpaces), 16);
}

ArithmeticType NumberFormat::fromOctal(const std::string &s, int decimalSpaces) {
    return mpfr::mpreal(s, mpfr::digits2bits(decimalSpaces), 8);
}

ArithmeticType NumberFormat::fromBinary(const std::string &s, int decimalSpaces) {
    if (s.size() > 63)
        throw std::runtime_error("Maximum 64 bits can be converted from string");
    return std::bitset<64>(s).to_ullong();
}