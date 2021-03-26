#ifndef QT_CALC_SCRIPT_HPP
#define QT_CALC_SCRIPT_HPP

#include <string>

#include "extern/json.hpp"

struct Script {
    std::string name;
    std::string body;
    bool enableArguments = false; //If true the script is wrapped by a vararg function otherwise a function with 0 arguments is used.

    Script() = default;

    Script(std::string name, std::string body, bool enableArguments)
            : name(std::move(name)), body(std::move(body)), enableArguments(enableArguments) {}

    bool operator==(const Script &other) const {
        return name == other.name && body == other.body && enableArguments == other.enableArguments;
    }
};

#endif //QT_CALC_SCRIPT_HPP
