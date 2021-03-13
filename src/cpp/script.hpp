#ifndef QT_CALC_SCRIPT_HPP
#define QT_CALC_SCRIPT_HPP

#include <string>

#include "extern/json.hpp"

struct Script {
    std::string name;
    std::string body;
    size_t argCount;
};

#endif //QT_CALC_SCRIPT_HPP
