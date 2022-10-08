/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "symboltable.hpp"

SymbolTable::SymbolTable() = default;

const std::map<std::string, decimal::Decimal> &SymbolTable::getVariables() const {
    return variables;
}

const std::map<std::string, decimal::Decimal> &SymbolTable::getConstants() const {
    return constants;
}

const std::map<std::string, Function> &SymbolTable::getFunctions() const {
    return functions;
}

const std::map<std::string, Script> &SymbolTable::getScripts() const {
    return scripts;
}

void SymbolTable::setVariable(const std::string &name, decimal::Decimal value) {
    if (name.empty())
        throw std::runtime_error("Symbol name cannot be empty.");

    constants.erase(name);
    functions.erase(name);
    scripts.erase(name);
    variables[name] = value;
}

void SymbolTable::setConstant(const std::string &name, decimal::Decimal value) {
    if (name.empty())
        throw std::runtime_error("Symbol name cannot be empty.");

    variables.erase(name);
    functions.erase(name);
    scripts.erase(name);
    constants[name] = value;
}

void SymbolTable::setFunction(const std::string &name, const Function &value) {
    if (name.empty())
        throw std::runtime_error("Symbol name cannot be empty.");

    variables.erase(name);
    constants.erase(name);
    scripts.erase(name);
    functions[name] = value;
}

void SymbolTable::setScript(const std::string &name, const Script &value) {
    if (name.empty())
        throw std::runtime_error("Symbol name cannot be empty.");

    variables.erase(name);
    constants.erase(name);
    functions.erase(name);
    scripts[name] = value;
}

bool SymbolTable::hasVariable(const std::string &name) {
    return variables.find(name) != variables.end();
}

bool SymbolTable::hasConstant(const std::string &name) {
    return constants.find(name) != constants.end();
}

bool SymbolTable::hasFunction(const std::string &name) {
    return functions.find(name) != functions.end();
}

bool SymbolTable::hasScript(const std::string &name) {
    return scripts.find(name) != scripts.end();
}

void SymbolTable::remove(const std::string &name) {
    if (name.empty())
        throw std::runtime_error("Symbol name cannot be empty.");

    variables.erase(name);
    constants.erase(name);
    functions.erase(name);
    scripts.erase(name);
}

void SymbolTable::clearVariables() {
    variables.clear();
}

void SymbolTable::clearConstants() {
    constants.clear();
}

void SymbolTable::clearFunctions() {
    functions.clear();
}

void SymbolTable::clearScripts() {
    scripts.clear();
}

bool SymbolTable::getUseBuiltInConstants() const {
    return useBuiltInConstants;
}

void SymbolTable::setUseBuiltInConstants(bool useBuiltIns) {
    useBuiltInConstants = useBuiltIns;
}
