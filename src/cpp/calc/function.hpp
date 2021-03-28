#ifndef QCALC_FUNCTION_HPP
#define QCALC_FUNCTION_HPP

#include <string>
#include <vector>

#include "extern/json.hpp"

struct Function {
    std::string expression;
    std::vector<std::string> argumentNames;

    Function() : expression(), argumentNames() {};

    Function(std::string expression, std::vector<std::string> arguments)
            : expression(std::move(expression)), argumentNames(std::move(arguments)) {}

    bool operator==(const Function &other) const {
        return expression == other.expression && argumentNames == other.argumentNames;
    }
};

#endif //QCALC_FUNCTION_HPP
