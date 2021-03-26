#include "symboltable.hpp"

SymbolTable::SymbolTable() = default;

SymbolTable::SymbolTable(const std::vector<Variable> &variables,
                         const std::vector<Constant> &constants,
                         const std::vector<Function> &functions,
                         const std::vector<Script> &scripts) {
    for (auto &v : variables) {
        addVariable(v);
    }
    for (auto &v : constants) {
        addConstant(v);
    }
    for (auto &v : functions) {
        addFunction(v);
    }
    for (auto &v : scripts) {
        addScript(v);
    }
}

void SymbolTable::setVariables(const std::vector<Variable> &value) {
    std::vector<Variable> tmp = this->variables;
    this->variables.clear();
    for (auto &v : value) {
        try {
            addVariable(v);
        }
        catch (std::exception &e) {
            this->variables = tmp;
            throw e;
        }
    }
}

const std::vector<Variable> &SymbolTable::getVariables() const {
    return variables;
}

void SymbolTable::setConstants(const std::vector<Constant> &value) {
    std::vector<Constant> tmp = this->constants;
    this->constants.clear();
    for (auto &v : value) {
        try {
            addConstant(v);
        }
        catch (std::exception &e) {
            this->constants = tmp;
            throw e;
        }
    }
}

const std::vector<Constant> &SymbolTable::getConstants() const {
    return constants;
}

void SymbolTable::setFunctions(const std::vector<Function> &value) {
    std::vector<Function> tmp = this->functions;
    this->functions.clear();
    for (auto &v : value) {
        try {
            addFunction(v);
        }
        catch (std::exception &e) {
            this->functions = tmp;
            throw e;
        }
    }
}

const std::vector<Function> &SymbolTable::getFunctions() const {
    return functions;
}

void SymbolTable::setScript(const std::vector<Script> &value) {
    std::vector<Script> tmp = this->scripts;
    this->scripts.clear();
    for (auto &v : value) {
        try {
            addScript(v);
        }
        catch (std::exception &e) {
            this->scripts = tmp;
            throw e;
        }
    }
}

const std::vector<Script> &SymbolTable::getScripts() const {
    return scripts;
}

void SymbolTable::addVariable(const Variable &variable) {
    std::string prefix = "Cannot add variable, ";
    if (hasVariable(variable.name))
        throw std::runtime_error(prefix + "Variable with name " + variable.name + " already exists.");
    if (hasConstant(variable.name))
        throw std::runtime_error(prefix + "Constant with name " + variable.name + " already exists");
    if (hasFunction(variable.name))
        throw std::runtime_error(prefix + "Function with name " + variable.name + " already exists");
    if (hasScript(variable.name))
        throw std::runtime_error(prefix + "Script with name " + variable.name + " already exists");

    variables.emplace_back(variable);
}

Variable SymbolTable::getVariable(size_t index) const {
    return variables.at(index);
}

void SymbolTable::setVariable(size_t index, const Variable &variable) {
    std::string prefix = "Cannot set variable, ";
    if (hasVariable(variable.name) && variables.at(index).name != variable.name)
        throw std::runtime_error(prefix + "Variable with name " + variable.name + " already exists.");
    if (hasConstant(variable.name))
        throw std::runtime_error(prefix + "Constant with name " + variable.name + " already exists");
    if (hasFunction(variable.name))
        throw std::runtime_error(prefix + "Function with name " + variable.name + " already exists");
    if (hasScript(variable.name))
        throw std::runtime_error(prefix + "Script with name " + variable.name + " already exists");

    variables.at(index) = variable;
}

size_t SymbolTable::getVariableCount() const {
    return variables.size();
}

void SymbolTable::removeVariable(size_t index) {
    if (index >= variables.size())
        throw std::runtime_error("Index out of range");

    variables.erase(variables.begin() + index);
}

bool SymbolTable::hasVariable(const std::string &name) const {
    for (auto &v : variables) {
        if (v.name == name)
            return true;
    }
    return false;
}

void SymbolTable::addConstant(const Constant &constant) {
    std::string prefix = "Cannot add constant, ";
    if (hasVariable(constant.name))
        throw std::runtime_error(prefix + "Variable with name " + constant.name + " already exists.");
    if (hasConstant(constant.name))
        throw std::runtime_error(prefix + "Constant with name " + constant.name + " already exists");
    if (hasFunction(constant.name))
        throw std::runtime_error(prefix + "Function with name " + constant.name + " already exists");
    if (hasScript(constant.name))
        throw std::runtime_error(prefix + "Script with name " + constant.name + " already exists");

    constants.emplace_back(constant);
}

Constant SymbolTable::getConstant(size_t index) const {
    return constants.at(index);
}

void SymbolTable::setConstant(size_t index, const Constant &constant) {
    std::string prefix = "Cannot set constant, ";
    if (hasVariable(constant.name))
        throw std::runtime_error(prefix + "Variable with name " + constant.name + " already exists.");
    if (hasConstant(constant.name) && constants.at(index).name != constant.name)
        throw std::runtime_error(prefix + "Constant with name " + constant.name + " already exists");
    if (hasFunction(constant.name))
        throw std::runtime_error(prefix + "Function with name " + constant.name + " already exists");
    if (hasScript(constant.name))
        throw std::runtime_error(prefix + "Script with name " + constant.name + " already exists");

    constants.at(index) = constant;
}

size_t SymbolTable::getConstantCount() const {
    return constants.size();
}

void SymbolTable::removeConstant(size_t index) {
    if (index >= constants.size())
        throw std::runtime_error("Index out of range");

    constants.erase(constants.begin() + index);
}

bool SymbolTable::hasConstant(const std::string &name) const {
    for (auto &v : constants) {
        if (v.name == name)
            return true;
    }
    return false;
}

void SymbolTable::addFunction(const Function &function) {
    std::string prefix = "Cannot add function, ";
    if (hasVariable(function.name))
        throw std::runtime_error(prefix + "Variable with name " + function.name + " already exists.");
    if (hasConstant(function.name))
        throw std::runtime_error(prefix + "Constant with name " + function.name + " already exists");
    if (hasFunction(function.name))
        throw std::runtime_error(prefix + "Function with name " + function.name + " already exists");
    if (hasScript(function.name))
        throw std::runtime_error(prefix + "Script with name " + function.name + " already exists");

    functions.emplace_back(function);
}

Function SymbolTable::getFunction(size_t index) const {
    return functions.at(index);
}

void SymbolTable::setFunction(size_t index, const Function &function) {
    std::string prefix = "Cannot set function, ";
    if (hasVariable(function.name))
        throw std::runtime_error(prefix + "Variable with name " + function.name + " already exists.");
    if (hasConstant(function.name))
        throw std::runtime_error(prefix + "Constant with name " + function.name + " already exists");
    if (hasFunction(function.name) && functions.at(index).name != function.name)
        throw std::runtime_error(prefix + "Function with name " + function.name + " already exists");
    if (hasScript(function.name))
        throw std::runtime_error(prefix + "Script with name " + function.name + " already exists");

    functions.at(index) = function;
}

size_t SymbolTable::getFunctionCount() const {
    return functions.size();
}

void SymbolTable::removeFunction(size_t index) {
    if (index >= functions.size())
        throw std::runtime_error("Index out of range");

    functions.erase(functions.begin() + index);
}

bool SymbolTable::hasFunction(const std::string &name) const {
    for (auto &v : functions) {
        if (v.name == name)
            return true;
    }
    return false;
}

void SymbolTable::addScript(const Script &script) {
    std::string prefix = "Cannot add script, ";
    if (hasVariable(script.name))
        throw std::runtime_error(prefix + "Variable with name " + script.name + " already exists.");
    if (hasConstant(script.name))
        throw std::runtime_error(prefix + "Constant with name " + script.name + " already exists");
    if (hasFunction(script.name))
        throw std::runtime_error(prefix + "Function with name " + script.name + " already exists");
    if (hasScript(script.name))
        throw std::runtime_error(prefix + "Script with name " + script.name + " already exists");

    scripts.emplace_back(script);
}

Script SymbolTable::getScript(size_t index) const {
    return scripts.at(index);
}

void SymbolTable::setScript(size_t index, const Script &script) {
    std::string prefix = "Cannot set script, ";
    if (hasVariable(script.name))
        throw std::runtime_error(prefix + "Variable with name " + script.name + " already exists.");
    if (hasConstant(script.name))
        throw std::runtime_error(prefix + "Constant with name " + script.name + " already exists");
    if (hasFunction(script.name))
        throw std::runtime_error(prefix + "Function with name " + script.name + " already exists");
    if (hasScript(script.name) && scripts.at(index).name != script.name)
        throw std::runtime_error(prefix + "Script with name " + script.name + " already exists");

    scripts.at(index) = script;
}

size_t SymbolTable::getScriptCount() const {
    return scripts.size();
}

void SymbolTable::removeScript(size_t index) {
    if (index >= scripts.size())
        throw std::runtime_error("Index out of range");

    scripts.erase(scripts.begin() + index);
}

bool SymbolTable::hasScript(const std::string &name) const {
    for (auto &v : scripts) {
        if (v.name == name)
            return true;
    }
    return false;
}
