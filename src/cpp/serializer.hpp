#ifndef QT_CALC_SERIALIZER_HPP
#define QT_CALC_SERIALIZER_HPP

#include <string>

#include "state.hpp"

class Serializer {
public:
    static std::string serializeTable(const SymbolTable &table);

    static SymbolTable deserializeTable(const std::string &str);

    static std::string serializeSettings(const State &state);

    static State deserializeSettings(const std::string &str);
};

#endif //QT_CALC_SERIALIZER_HPP
