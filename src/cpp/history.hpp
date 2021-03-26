#ifndef QCALC_HISTORY_HPP
#define QCALC_HISTORY_HPP

#include <vector>
#include <string>

#include "valuetype.hpp"
#include "numberformat.hpp"

class History {
public:
    void add(const std::string &expression, ValueType result) {
        data.emplace_back(std::pair<std::string, ValueType>(expression, result));
    }

    std::pair<std::string, ValueType> get(size_t index) {
        return data.at(index);
    }

    const std::vector<std::pair<std::string, ValueType>> &getVector() {
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
    std::vector<std::pair<std::string, ValueType>> data;
};

#endif //QCALC_HISTORY_HPP
