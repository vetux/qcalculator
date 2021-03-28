#include "calc/symboltable.hpp"

SymbolTable::SymbolTable() = default;

const std::map<std::string, ArithmeticType> &SymbolTable::getVariables() const {
    return variables;
}

const std::map<std::string, ArithmeticType> &SymbolTable::getConstants() const {
    return constants;
}

const std::map<std::string, Function> &SymbolTable::getFunctions() const {
    return functions;
}

const std::map<std::string, Script> &SymbolTable::getScripts() const {
    return scripts;
}

void SymbolTable::setVariable(const std::string &name, ArithmeticType value) {
    if (name.empty())
        throw std::runtime_error("Symbol name cannot be empty.");

    constants.erase(name);
    functions.erase(name);
    scripts.erase(name);
    variables[name] = value;
}

void SymbolTable::setConstant(const std::string &name, ArithmeticType value) {
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