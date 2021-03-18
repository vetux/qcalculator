#ifndef QCALC_SYMBOLTABLE_HPP
#define QCALC_SYMBOLTABLE_HPP

#include "variable.hpp"
#include "constant.hpp"
#include "function.hpp"
#include "script.hpp"

struct SymbolTable {
    std::vector<Variable> variables;
    std::vector<Constant> constants;
    std::vector<Function> functions;
    std::vector<Script> scripts;
};

#endif //QCALC_SYMBOLTABLE_HPP
