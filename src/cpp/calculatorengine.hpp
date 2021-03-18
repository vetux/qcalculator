#ifndef CALCULATORENGINE_HPP
#define CALCULATORENGINE_HPP

#include "scriptfunction.hpp"

#include <string>
#include <map>

#include "symboltable.hpp"

/**
 * The calculator engine supports functions and scripts.
 *
 * Functions are executed as native c++ using the templated type for arithmetic but may only contain syntax defined by the exprtk readme.
 *
 * Scripts pass through the python interpreter and may therefore only be capable of limited arithmetic accuracy and performance compared to functions,
 * but allow full python functionality with io etc.
 *
 * The ScriptFunction mapping is interpreted as the keys beeing the function names as invoked in the expressions
 * and the values beeing the python scripts invoked in response.
 *
 * Arguments are passed to the scripts via argv.
 *
 */
class CalculatorEngine {
public:
    CalculatorEngine();

    ~CalculatorEngine();

    /**
     * Evaluate the arithmetic expression using the defined symbol table.
     *
     * @param expr The mathematical expression which may contain symbolTable defined in the table.
     * @param symbolTable The symbol table to use when evaluating the expression.
     *
     * @return The value of the expression.
     */
    ValueType evaluate(const std::string &expr, SymbolTable &symbolTable);

    ValueType evaluate(const std::string &expr);

private:
    PythonInterpreter interpreter;
};

#endif // CALCULATORENGINE_HPP
