#ifndef QCALC_CONSTANT_HPP
#define QCALC_CONSTANT_HPP

#include <string>
#include <utility>

#include "valuetype.hpp"

struct Constant {
    std::string name;
    ValueType value;

    Constant() : name(), value() {};

    Constant(std::string name, ValueType value) : name(std::move(name)), value(value) {}

    bool operator==(const Constant &other) const {
        return name == other.name && value == other.value;
    }
};

#endif //QCALC_CONSTANT_HPP
