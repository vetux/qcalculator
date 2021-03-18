#ifndef QCALC_VARIABLE_HPP
#define QCALC_VARIABLE_HPP

#include <string>

#include "valuetype.hpp"

struct Variable {
    std::string name;
    ValueType value;

    Variable() = default;

    Variable(std::string name, ValueType value) : name(name), value(value) {}
};

#endif //QCALC_VARIABLE_HPP
