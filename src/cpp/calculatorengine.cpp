#include "calculatorengine.hpp"

#include "extern/exprtk.hpp"


CalculatorEngine::CalculatorEngine() = default;

CalculatorEngine::~CalculatorEngine() = default;

ValueType CalculatorEngine::evaluate(const std::string &expr, SymbolTable &symbolTable) {
    exprtk::function_compositor<ValueType> compositor;
    exprtk::symbol_table<ValueType> symbols = compositor.symbol_table();

    int varArgScriptCount = 0;
    int scriptCount = 0;
    for (auto &script : symbolTable.scripts) {
        if (script.enableArguments)
            varArgScriptCount++;
        else
            scriptCount++;
    }

    //Use vectors with fixed size to store the function objects as the symbol table itself only stores references.
    int varArgScriptIndex = 0;
    std::vector<ScriptVarArgFunction<ValueType>> varArgScriptFunctions;
    varArgScriptFunctions.resize(varArgScriptCount);

    int scriptIndex = 0;
    std::vector<ScriptFunction<ValueType>> scriptFunctions;
    scriptFunctions.resize(scriptCount);

    for (auto &script : symbolTable.scripts) {
        if (script.enableArguments) {
            int index = varArgScriptIndex++;
            assert(index < varArgScriptCount);
            varArgScriptFunctions.at(index) = ScriptVarArgFunction<ValueType>(interpreter, script.body);
            symbols.add_function(script.name, varArgScriptFunctions.at(index));
        } else {
            int index = scriptIndex++;
            assert(index < scriptCount);
            scriptFunctions.at(index) = ScriptFunction<ValueType>(interpreter, script.body);
            symbols.add_function(script.name, scriptFunctions.at(index));
        }
    }

    assert(varArgScriptIndex == varArgScriptCount);
    assert(scriptIndex == scriptCount);

    for (auto &func : symbolTable.functions) {
        switch (func.argumentNames.size()) {
            case 0:
                compositor.add(typename exprtk::function_compositor<ValueType>::function(func.name, func.expression));
                break;
            case 1:
                compositor.add(typename exprtk::function_compositor<ValueType>::function(func.name, func.expression,
                                                                                         func.argumentNames[0]));
                break;
            case 2:
                compositor.add(
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression,
                                                                                  func.argumentNames[0],
                                                                                  func.argumentNames[1]));
                break;
            case 3:
                compositor.add(
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression,
                                                                                  func.argumentNames[0],
                                                                                  func.argumentNames[1],
                                                                                  func.argumentNames[2]));
                break;
            case 4:
                compositor.add(
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression,
                                                                                  func.argumentNames[0],
                                                                                  func.argumentNames[1],
                                                                                  func.argumentNames[2],
                                                                                  func.argumentNames[3]));
                break;
            case 5:
                compositor.add(
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression,
                                                                                  func.argumentNames[0],
                                                                                  func.argumentNames[1],
                                                                                  func.argumentNames[2],
                                                                                  func.argumentNames[3],
                                                                                  func.argumentNames[4]));
                break;
            default:
                throw std::runtime_error("Too many function argumentNames");
        }
    }

    for (auto &constant : symbolTable.constants) {
        symbols.add_constant(constant.name, constant.value);
    }

    for (auto &variable : symbolTable.variables) {
        symbols.add_variable(variable.name, variable.value);
    }

    exprtk::expression<ValueType> expression;
    expression.register_symbol_table(symbols);

    if (parser.compile(expr, expression)) {
        return expression.value();
    } else {
        throw std::runtime_error(parser.error());
    }
}

ValueType CalculatorEngine::evaluate(const std::string &expr) {
    exprtk::expression<ValueType> expression;

    if (parser.compile(expr, expression)) {
        return expression.value();
    } else {
        throw std::runtime_error("Failed to compile expression");
    }
}