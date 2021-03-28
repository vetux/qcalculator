#ifndef QCALC_HISTORY_HPP
#define QCALC_HISTORY_HPP

#include <vector>
#include <string>

#include "numberformat.hpp"

class History {
public:
    void add(const std::string &expression, ArithmeticType result) {
        data.emplace_back(std::pair<std::string, ArithmeticType>(expression, result));
    }

    std::pair<std::string, ArithmeticType> get(size_t index) {
        return data.at(index);
    }

    const std::vector<std::pair<std::string, ArithmeticType>> &getVector() {
        return data;
    }

    std::vector<std::pair<std::string, std::string>> getVectorWithDecimalStringValues() {
        std::vector<std::pair<std::string, std::string>> ret;
        for (auto &v : data) {
            ret.emplace_back(std::pair<std::string, std::string>(v.first, NumberFormat::toDecimal(v.second)));
        }
        return ret;
    }

private:
    std::vector<std::pair<std::string, ArithmeticType>> data;
};

#endif //QCALC_HISTORY_HPP
