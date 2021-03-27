#ifndef QCALC_SYMBOLTABLE_HPP
#define QCALC_SYMBOLTABLE_HPP

#include <map>

#include "variable.hpp"
#include "constant.hpp"
#include "function.hpp"
#include "script.hpp"
#include "uuid.hpp"
#include "uuidgenerator.hpp"

class SymbolTable {
public:
    SymbolTable();


    const std::map<UUID, Variable> &getVariables() const;

    const std::map<UUID, Constant> &getConstants() const;

    const std::map<UUID, Function> &getFunctions() const;

    const std::map<UUID, Script> &getScripts() const;


    UUID addVariable(const Variable &variable);

    const Variable &getVariable(UUID id) const;

    void setVariable(UUID id, const Variable &variable);

    void removeVariable(UUID id);

    bool hasVariable(const std::string &name) const;

    void clearVariables();


    UUID addConstant(const Constant &constant);

    const Constant &getConstant(UUID id) const;

    void setConstant(UUID id, const Constant &constant);

    void removeConstant(UUID id);

    bool hasConstant(const std::string &name) const;

    void clearConstants();


    UUID addFunction(const Function &function);

    const Function &getFunction(UUID id) const;

    void setFunction(UUID id, const Function &function);

    void removeFunction(UUID id);

    bool hasFunction(const std::string &name) const;

    void clearFunctions();


    UUID addScript(const Script &script);

    const Script &getScript(UUID id) const;

    void setScript(UUID id, const Script &script);

    void removeScript(UUID id);

    bool hasScript(const std::string &name) const;

    void clearScripts();

private:
    UUIDGenerator generator;

    std::map<UUID, Variable> variables;
    std::map<UUID, Constant> constants;
    std::map<UUID, Function> functions;
    std::map<UUID, Script> scripts;
};

#endif //QCALC_SYMBOLTABLE_HPP
