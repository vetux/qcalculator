/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_SYMBOLTABLE_HPP
#define QCALC_SYMBOLTABLE_HPP

#include <map>
#include <string>

#include <decimal.hh>

#include "math/function.hpp"
#include "math/script.hpp"

/**
 * The symbol table is responsible for managing 4 map objects.
 * Each symbol(Variable, Constant, Function and Script) is identified by a name.
 *
 * A variable name cannot be a empty string.
 *
 * Only one symbol type per name may exist.
 * When setting a symbol of an existing name with different type the original symbol is deleted.
 */
class SymbolTable {
public:
    static const std::map<std::string, std::string> &getBuiltIns();

    SymbolTable();

    SymbolTable(const SymbolTable &other) = default;

    SymbolTable(SymbolTable &&other) = default;

    SymbolTable &operator=(const SymbolTable &other) = default;

    SymbolTable &operator=(SymbolTable &&other) = default;

    bool getUseBuiltInConstants() const;

    const std::map<std::string, decimal::Decimal> &getVariables() const;

    const std::map<std::string, decimal::Decimal> &getConstants() const;

    const std::map<std::string, Function> &getFunctions() const;

    const std::map<std::string, Script> &getScripts() const;

    void setUseBuiltInConstants(bool useBuiltIns);

    void setVariable(const std::string &name, const decimal::Decimal &value);

    void setConstant(const std::string &name, const decimal::Decimal &value);

    void setFunction(const std::string &name, const Function &value);

    void setScript(const std::string &name, const Script &value);

    bool hasVariable(const std::string &name);

    bool hasConstant(const std::string &name);

    bool hasFunction(const std::string &name);

    bool hasScript(const std::string &name);

    void remove(const std::string &name);

    void clearVariables();

    void clearConstants();

    void clearFunctions();

    void clearScripts();

    bool equals(const SymbolTable &other) const {
        return useBuiltInConstants == other.useBuiltInConstants
               && variables == other.variables
               && constants == other.constants
               && functions == other.functions
               && scripts == other.scripts;
    }

    bool equalsExcludeScripts(const SymbolTable &other) const {
        return useBuiltInConstants == other.useBuiltInConstants
               && variables == other.variables
               && constants == other.constants
               && functions == other.functions;
    }

private:
    bool useBuiltInConstants = true;
    std::map<std::string, decimal::Decimal> variables;
    std::map<std::string, decimal::Decimal> constants;
    std::map<std::string, Function> functions;
    std::map<std::string, Script> scripts;
};

#endif //QCALC_SYMBOLTABLE_HPP
