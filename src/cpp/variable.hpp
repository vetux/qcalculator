#ifndef QCALC_VARIABLE_HPP
#define QCALC_VARIABLE_HPP

#include <string>

#include "valuetype.hpp"

struct Variable {
    std::string name;
    ValueType value = 0;

    Variable() = default;

    Variable(std::string name, ValueType value) : name(std::move(name)), value(value) {}
};

#endif //QCALC_VARIABLE_HPP
