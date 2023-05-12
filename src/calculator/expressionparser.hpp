/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_EXPRESSIONPARSER_HPP
#define QCALC_EXPRESSIONPARSER_HPP

#include <string>

#include <decimal.hh>

#include "calculator/symboltable.hpp"

/**
 * The expression parser evaluates expressions in string form using an optionally supplied symbol table.
 *
 * The symbol table defines variables, constants, functions and scripts.
 *
 * Variables can be changed from expressions using a special syntax and these changes are stored in the passed symbol table.
 * Functions are implemented using exprtk's function_compositor.
 * Scripts are implemented as a custom exprtk function.
 */
namespace ExpressionParser {
    /**
     * Evaluate the arithmetic expression using the defined symbol table.
     *
     * @param expr The mathematical expression which may contain symbols defined in the table.
     * @param symbolTable The symbol table to use when evaluating the expression.
     *
     * @return The value of the expression.
     */
    decimal::Decimal evaluate(const std::string &expr, SymbolTable &symbolTable);

    decimal::Decimal evaluate(const std::string &expr);
}

#endif // QCALC_EXPRESSIONPARSER_HPP
