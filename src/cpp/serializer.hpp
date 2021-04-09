#ifndef QT_CALC_SERIALIZER_HPP
#define QT_CALC_SERIALIZER_HPP

#include <string>
#include <set>

#include "math/symboltable.hpp"

#include "settings.hpp"

namespace Serializer {
    std::string serializeTable(const SymbolTable &table);

    SymbolTable deserializeTable(const std::string &str);

    std::string serializeSettings(const Settings &settings);

    Settings deserializeSettings(const std::string &str);

    std::string serializeSet(const std::set<std::string> &set);

    std::set<std::string> deserializeSet(const std::string &str);
}

#endif //QT_CALC_SERIALIZER_HPP
