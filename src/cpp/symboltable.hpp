#ifndef QCALC_SYMBOLTABLE_HPP
#define QCALC_SYMBOLTABLE_HPP

#include <map>

#include "variable.hpp"
#include "constant.hpp"
#include "function.hpp"
#include "script.hpp"
#include "uid.hpp"
#include "uidgenerator.hpp"

class SymbolTable {
public:
    SymbolTable();


    const std::map<UID, Variable> &getVariables() const;

    const std::map<UID, Constant> &getConstants() const;

    const std::map<UID, Function> &getFunctions() const;

    const std::map<UID, Script> &getScripts() const;


    UID addVariable(const Variable &variable);

    const Variable &getVariable(UID id) const;

    void setVariable(UID id, const Variable &variable);

    void removeVariable(UID id);

    bool hasVariable(const std::string &name) const;

    void clearVariables();


    UID addConstant(const Constant &constant);

    const Constant &getConstant(UID id) const;

    void setConstant(UID id, const Constant &constant);

    void removeConstant(UID id);

    bool hasConstant(const std::string &name) const;

    void clearConstants();


    UID addFunction(const Function &function);

    const Function &getFunction(UID id) const;

    void setFunction(UID id, const Function &function);

    void removeFunction(UID id);

    bool hasFunction(const std::string &name) const;

    void clearFunctions();


    UID addScript(const Script &script);

    const Script &getScript(UID id) const;

    void setScript(UID id, const Script &script);

    void removeScript(UID id);

    bool hasScript(const std::string &name) const;

    void clearScripts();

private:
    UIDGenerator generator;

    std::map<UID, Variable> variables;
    std::map<UID, Constant> constants;
    std::map<UID, Function> functions;
    std::map<UID, Script> scripts;
};

#endif //QCALC_SYMBOLTABLE_HPP
