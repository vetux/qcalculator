#ifndef QCALC_CONSTANT_HPP
#define QCALC_CONSTANT_HPP

#include <string>

#include "valuetype.hpp"

struct Constant {
    std::string name;
    ValueType value;

    Constant() = default;

    Constant(std::string name, ValueType value) : name(name), value(value) {}
};

#endif //QCALC_CONSTANT_HPP
