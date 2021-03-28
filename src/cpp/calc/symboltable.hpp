#ifndef QCALC_SYMBOLTABLE_HPP
#define QCALC_SYMBOLTABLE_HPP

#include <map>
#include <string>

#include "calc/function.hpp"
#include "calc/script.hpp"
#include "calc/arithmetictype.hpp"

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
    SymbolTable();

    const std::map<std::string, ArithmeticType> &getVariables() const;

    const std::map<std::string, ArithmeticType> &getConstants() const;

    const std::map<std::string, Function> &getFunctions() const;

    const std::map<std::string, Script> &getScripts() const;

    void setVariable(const std::string &name, ArithmeticType value);

    void setConstant(const std::string &name, ArithmeticType value);

    void setFunction(const std::string &name, const Function &value);

    void setScript(const std::string &name, const Script &value);

    bool hasVariable(const std::string &name);

    bool hasConstant(const std::string &name);

    bool hasFunction(const std::string &name);

    bool hasScript(const std::string &name);

    void remove(const std::string &name);

private:
    std::map<std::string, ArithmeticType> variables;
    std::map<std::string, ArithmeticType> constants;
    std::map<std::string, Function> functions;
    std::map<std::string, Script> scripts;
};

#endif //QCALC_SYMBOLTABLE_HPP
