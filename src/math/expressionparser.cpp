/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
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

#include "expressionparser.hpp"

#include "extern/exprtk_mpdecimal_adaptor.hpp"
#include "extern/exprtk.hpp"

#include "scriptfunction.hpp"
#include "scriptvarargfunction.hpp"

decimal::Decimal ExpressionParser::evaluate(const std::string &expr, SymbolTable &symbolTable) {
    exprtk::parser<decimal::Decimal> parser;
    exprtk::function_compositor<decimal::Decimal> compositor;
    exprtk::symbol_table<decimal::Decimal> symbols = compositor.symbol_table();

    int varArgScriptCount = 0;
    int scriptCount = 0;
    for (auto &v: symbolTable.getScripts()) {
        if (v.second.arguments.empty())
            scriptCount++;
        else
            varArgScriptCount++;
    }

    //Use vectors with fixed size to store the function objects as the symbol table itself only stores references.
    int varArgScriptIndex = 0;
    std::vector<ScriptVarArgFunction<decimal::Decimal>> varArgScriptFunctions;
    varArgScriptFunctions.resize(varArgScriptCount);

    int scriptIndex = 0;
    std::vector<ScriptFunction<decimal::Decimal>> scriptFunctions;
    scriptFunctions.resize(scriptCount);

    for (auto &v: symbolTable.getScripts()) {
        if (v.second.arguments.empty()) {
            int index = scriptIndex++;
            assert(index < scriptCount);
            scriptFunctions.at(index) = ScriptFunction<decimal::Decimal>(v.second.callback);
            symbols.add_function(v.first, scriptFunctions.at(index));
        } else {
            int index = varArgScriptIndex++;
            assert(index < varArgScriptCount);
            varArgScriptFunctions.at(index) = ScriptVarArgFunction<decimal::Decimal>(v.second.callback);
            symbols.add_function(v.first, varArgScriptFunctions.at(index));
        }
    }

    assert(varArgScriptIndex == varArgScriptCount);
    assert(scriptIndex == scriptCount);

    for (auto &v: symbolTable.getFunctions()) {
        switch (v.second.argumentNames.size()) {
            case 0:
                compositor.add(
                        typename exprtk::function_compositor<decimal::Decimal>::function(v.first, v.second.expression));
                break;
            case 1:
                compositor.add(typename exprtk::function_compositor<decimal::Decimal>::function(v.first,
                                                                                              v.second.expression,
                                                                                              v.second.argumentNames[0]));
                break;
            case 2:
                compositor.add(
                        typename exprtk::function_compositor<decimal::Decimal>::function(v.first,
                                                                                       v.second.expression,
                                                                                       v.second.argumentNames[0],
                                                                                       v.second.argumentNames[1]));
                break;
            case 3:
                compositor.add(
                        typename exprtk::function_compositor<decimal::Decimal>::function(v.first,
                                                                                       v.second.expression,
                                                                                       v.second.argumentNames[0],
                                                                                       v.second.argumentNames[1],
                                                                                       v.second.argumentNames[2]));
                break;
            case 4:
                compositor.add(
                        typename exprtk::function_compositor<decimal::Decimal>::function(v.first,
                                                                                       v.second.expression,
                                                                                       v.second.argumentNames[0],
                                                                                       v.second.argumentNames[1],
                                                                                       v.second.argumentNames[2],
                                                                                       v.second.argumentNames[3]));
                break;
            case 5:
                compositor.add(
                        typename exprtk::function_compositor<decimal::Decimal>::function(v.first,
                                                                                       v.second.expression,
                                                                                       v.second.argumentNames[0],
                                                                                       v.second.argumentNames[1],
                                                                                       v.second.argumentNames[2],
                                                                                       v.second.argumentNames[3],
                                                                                       v.second.argumentNames[4]));
                break;
            default:
                throw std::runtime_error("Too many function argumentNames");
        }
    }

    for (auto &constant: symbolTable.getConstants()) {
        symbols.add_constant(constant.first, constant.second);
    }

    std::map<std::string, decimal::Decimal> variables = symbolTable.getVariables();
    for (auto &variable: variables) {
        symbols.add_variable(variable.first, variable.second);
    }

    if (symbolTable.getUseBuiltInConstants()) {
        symbols.add_constants();
    }

    exprtk::expression<decimal::Decimal> expression;
    expression.register_symbol_table(symbols);

    if (parser.compile(expr, expression)) {
        decimal::Decimal ret = expression.value();
        for (auto &v: variables) {
            if (symbolTable.getVariables().at(v.first) == v.second)
                continue;
            symbolTable.setVariable(v.first, v.second);
        }
        return ret;
    } else {
        throw std::runtime_error(parser.error());
    }
}

decimal::Decimal ExpressionParser::evaluate(const std::string &expr) {
    exprtk::parser<decimal::Decimal> parser;
    exprtk::expression<decimal::Decimal> expression;

    if (parser.compile(expr, expression)) {
        return expression.value();
    } else {
        throw std::runtime_error(parser.error());
    }
}