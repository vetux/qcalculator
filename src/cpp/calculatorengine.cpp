#include "calculatorengine.hpp"

#include "extern/exprtk.hpp"


CalculatorEngine::CalculatorEngine() {
}

CalculatorEngine::~CalculatorEngine() {

}

ValueType CalculatorEngine::evaluate(const std::string &expr, SymbolTable &symbolTable) {
    exprtk::parser<ValueType> parser;
    exprtk::function_compositor<ValueType> compositor;
    exprtk::symbol_table<ValueType> symbols = compositor.symbol_table();

    std::vector<ScriptFunction<ValueType>> funcs;

    for (auto &script : symbolTable.scripts) {
        ScriptFunction<ValueType> func = ScriptFunction<ValueType>(interpreter, script.body);
        funcs.emplace_back(func);
        symbols.add_function(script.name, *(funcs.end() - 1));
    }

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
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression, func.argumentNames[0],
                                                                          func.argumentNames[1]));
                break;
            case 3:
                compositor.add(
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression, func.argumentNames[0],
                                                                          func.argumentNames[1],
                                                                          func.argumentNames[2]));
                break;
            case 4:
                compositor.add(
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression, func.argumentNames[0],
                                                                          func.argumentNames[1],
                                                                          func.argumentNames[2], func.argumentNames[3]));
                break;
            case 5:
                compositor.add(
                        typename exprtk::function_compositor<ValueType>::function(func.name, func.expression, func.argumentNames[0],
                                                                          func.argumentNames[1],
                                                                          func.argumentNames[2], func.argumentNames[3],
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
    exprtk::parser<ValueType> parser;
    exprtk::expression<ValueType> expression;

    if (parser.compile(expr, expression)) {
        return expression.value();
    } else {
        throw std::runtime_error("Failed to compile expression");
    }
}