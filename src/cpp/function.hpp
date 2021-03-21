#ifndef QT_CALC_FUNCTION_HPP
#define QT_CALC_FUNCTION_HPP

#include <string>
#include <vector>

#include "extern/json.hpp"

struct Function {
    std::string name;
    std::string expression;
    std::vector<std::string> argumentNames;

    Function() = default;

    Function(std::string name, std::string expression, std::vector<std::string> arguments)
            : name(std::move(name)), expression(std::move(expression)), argumentNames(std::move(arguments)) {}
};

#endif //QT_CALC_FUNCTION_HPP
