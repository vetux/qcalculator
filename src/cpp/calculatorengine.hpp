#ifndef CALCULATORENGINE_HPP
#define CALCULATORENGINE_HPP

#include "scriptvarargfunction.hpp"
#include "scriptfunction.hpp"

#include <string>
#include <map>

#include "symboltable.hpp"

/**
 * The calculator engine evaluates expressions in string form using a supplied symbol table.
 *
 * The symbol table defines variables, constants, functions and scripts.
 *
 * Functions are implemented using exprtk's function_compositor.
 *
 * Scripts are implemented as a custom exprtk function.
 *
 * Scripts get access to arguments through the variable "argv" and may output a single scalar by setting "outv".
 */
class CalculatorEngine {
public:
    CalculatorEngine();

    ~CalculatorEngine();

    /**
     * Evaluate the arithmetic expression using the defined symbol table.
     *
     * @param expr The mathematical expression which may contain symbols defined in the table.
     * @param symbolTable The symbol table to use when evaluating the expression.
     *
     * @return The value of the expression.
     */
    ValueType evaluate(const std::string &expr, SymbolTable &symbolTable);

    ValueType evaluate(const std::string &expr);

private:
    exprtk::parser<ValueType> parser;
    PythonInterpreter interpreter;
};

#endif // CALCULATORENGINE_HPP
