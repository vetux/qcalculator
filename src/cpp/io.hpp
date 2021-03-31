#ifndef QCALC_IO_HPP
#define QCALC_IO_HPP

#include <string>

#include "settings.hpp"

#include "calc/symboltable.hpp"

namespace IO {
    std::string getAppDirectory();

    Settings loadSettings(const std::string &filePath);

    void saveSettings(const std::string &filePath, const Settings &settings);

    SymbolTable loadSymbolTable(const std::string &filePath);

    void saveSymbolTable(const std::string &filePath, const SymbolTable &symbolTable);
}

#endif //QCALC_IO_HPP
