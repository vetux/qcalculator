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
    static std::string toDecimal(ValueType v, int floatPrecision = 32) {
        std::stringstream stream;
        stream << std::fixed << std::setprecision(floatPrecision) << v;
        std::string s = stream.str();
        if (s.find('.') != std::string::npos) {
            for (size_t i = s.size() - 1; i >= 0; i--) {
                if (s[i] != '0' && s[i] != '.') {
                    return s.substr(0, i + 1);
                } else if (s[i] == '.') {
                    return s.substr(0, i);
                }
            }
        }
        return s;
    }

    static ValueType fromDecimal(const std::string &s) {
        return std::stold(s);
    }

    static std::string toHex(ValueType v) {
        if (v < 0) {
            return "Error sign";
        } else if (v > std::numeric_limits<long long>::max()) {
            return "Error range";
        } else if (hasFraction(v)) {
            return "Error fraction";
        } else {
            //Hex
            long long val = v;
            std::stringstream stream;
            stream << std::hex
                   << val; //Returns a large integer (?overflowed) as hex string for negative values. The reverse works without any problems.
            return stream.str();
        }
    }

    static ValueType fromHex(const std::string &s) {
        long long x;
        std::stringstream ss;
        ss << std::hex << s;
        ss >> x;
        return x;
    }

    static std::string toOctal(ValueType v) {
        if (v < 0) {
            return "Error sign";
        } else if (v > std::numeric_limits<long long>::max()) {
            return "Error range";
        } else if (hasFraction(v)) {
            return "Error fraction";
        } else {
            long long val = v;
            std::stringstream stream;
            stream << std::oct << val; //Same as with hex.
            return stream.str();
        }
    }

    static ValueType fromOctal(const std::string &s) {
        long long x;
        std::stringstream ss;
        ss << std::oct << s;
        ss >> x;
        return x;
    }

    static std::string toBinary(ValueType v) {
        if (v < 0) {
            return "Error sign";
        } else if (v > std::numeric_limits<long long>::max()) {
            return "Error range";
        } else if (hasFraction(v)) {
            return "Error fraction";
        } else {
            // Integer, normal binary
            std::string ret;
            std::string bitstring = std::bitset<sizeof(long long) * CHAR_BIT>((long long) v).to_string();
            for (size_t i = 0; i < bitstring.size(); i++) {
                if (bitstring.at(i) != '0') {
                    return bitstring.substr(i);
                }
            }
            return "0";
        }
    }

    static ValueType fromBinary(const std::string &s) {
        if (s.size() > 63)
            throw std::runtime_error("Bit limit");
        return std::bitset<64>(s).to_ullong();
    }
};

#endif // NUMERALREP_HPP
