#ifndef QCALC_EXPRESSIONPARSER_HPP
#define QCALC_EXPRESSIONPARSER_HPP

#include <string>


#include "calc/scriptvarargfunction.hpp"
#include "calc/scriptfunction.hpp"
#include "calc/symboltable.hpp"
#include "calc/arithmetictype.hpp"
#include "calc/pythonparser.hpp"

/**
 * The expression expressionParser evaluates expressions in string form using an optionally supplied symbol table.
 *
 * The symbol table defines variables, constants, functions and scripts.
 *
 * Variables can be changed from expressions using a special syntax and these changes are stored in the passed symbol table.
 *
 * Functions are implemented using exprtk's function_compositor.
 *
 * Scripts are implemented as a custom exprtk function.
 *
 * Scripts get access to arguments through the variable "argv" and may output a single scalar by setting "outv".
 *
 * @tparam T The type to use for arithmetics.
 */
class ExpressionParser {
public:
    ExpressionParser();

    ~ExpressionParser();

    /**
     * Evaluate the arithmetic expression using the defined symbol table.
     *
     * @param expr The mathematical expression which may contain symbols defined in the table.
     * @param symbolTable The symbol table to use when evaluating the expression.
     *
     * @return The value of the expression.
     */
    ArithmeticType evaluate(const std::string &expr, SymbolTable &symbolTable);

    ArithmeticType evaluate(const std::string &expr);

private:
    exprtk::parser<ArithmeticType> parser;
    PythonParser pythonParser;
};

#endif // QCALC_EXPRESSIONPARSER_HPP
