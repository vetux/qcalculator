#ifndef QCALC_SYMBOLTABLE_HPP
#define QCALC_SYMBOLTABLE_HPP

#include "variable.hpp"
#include "constant.hpp"
#include "function.hpp"
#include "script.hpp"

class SymbolTable {
public:
    SymbolTable();

    SymbolTable(const std::vector<Variable> &variables,
                const std::vector<Constant> &constants,
                const std::vector<Function> &functions,
                const std::vector<Script> &scripts);

    void setVariables(const std::vector<Variable> &value);

    const std::vector<Variable> &getVariables() const;

    void setConstants(const std::vector<Constant> &value);

    const std::vector<Constant> &getConstants() const;

    void setFunctions(const std::vector<Function> &value);

    const std::vector<Function> &getFunctions() const;

    void setScript(const std::vector<Script> &value);

    const std::vector<Script> &getScripts() const;


    void addVariable(const Variable &variable);

    Variable getVariable(size_t index) const;

    void setVariable(size_t index, const Variable &variable);

    size_t getVariableCount() const;

    void removeVariable(size_t index);

    bool hasVariable(const std::string &name) const;


    void addConstant(const Constant &constant);

    Constant getConstant(size_t index) const;

    void setConstant(size_t index, const Constant &constant);

    size_t getConstantCount() const;

    void removeConstant(size_t index);

    bool hasConstant(const std::string &name) const;


    void addFunction(const Function &function);

    Function getFunction(size_t index) const;

    void setFunction(size_t index, const Function &function);

    size_t getFunctionCount() const;

    void removeFunction(size_t index);

    bool hasFunction(const std::string &name) const;


    void addScript(const Script &script);

    Script getScript(size_t index) const;

    void setScript(size_t index, const Script &script);

    size_t getScriptCount() const;

    void removeScript(size_t index);

    bool hasScript(const std::string &name) const;

private:
    std::vector<Variable> variables;
    std::vector<Constant> constants;
    std::vector<Function> functions;
    std::vector<Script> scripts;
};

#endif //QCALC_SYMBOLTABLE_HPP
