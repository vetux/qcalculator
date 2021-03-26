#ifndef QCALC_VARIABLE_HPP
#define QCALC_VARIABLE_HPP

#include <string>

#include "valuetype.hpp"

struct Variable {
    std::string name;
    ValueType value;

    Variable() : name(), value() {};

    Variable(std::string name, ValueType value) : name(std::move(name)), value(value) {}

    bool operator==(const Variable &other) const {
        return name == other.name && value == other.value;
    }
};

#endif //QCALC_VARIABLE_HPP
