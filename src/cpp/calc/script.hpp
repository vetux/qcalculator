#ifndef QCALC_SCRIPT_HPP
#define QCALC_SCRIPT_HPP

#include <string>

/**
 * A Script is a python script which can be called from an expression and can have input variables and one output scalar.
 */
struct Script {
    std::string expression;
    bool enableArguments = false; //If true the script is wrapped by a vararg function otherwise a function with 0 arguments is used.

    Script() = default;

    Script(std::string expression, bool enableArguments)
            : expression(std::move(expression)), enableArguments(enableArguments) {}

    bool operator==(const Script &other) const {
        return expression == other.expression && enableArguments == other.enableArguments;
    }
};

#endif //QCALC_SCRIPT_HPP
