#ifndef CALCULATORENGINE_HPP
#define CALCULATORENGINE_HPP

#include "scriptfunction.hpp"

#include <string>
#include <map>

#include "function.hpp"
#include "script.hpp"

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
 * @tparam T The type to use for arithmetics.
 */
template<typename T>
class CalculatorEngine {
public:
    struct SymbolTable {
        std::map<std::string, T> variables;
        std::map<std::string, T> constants;
        std::map<std::string, Function> functions;
        std::map<std::string, Script> scripts;
    };

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
    T evaluate(const std::string &expr, SymbolTable &symbolTable);

    T evaluate(const std::string &expr);

private:
    PythonInterpreter interpreter;
};

template class CalculatorEngine<long double>;

#endif // CALCULATORENGINE_HPP
