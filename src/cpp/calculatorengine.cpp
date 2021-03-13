#include "calculatorengine.hpp"

#include "exprtk.hpp"

template<typename T>
CalculatorEngine<T>::CalculatorEngine() {
}

template<typename T>
CalculatorEngine<T>::~CalculatorEngine() {

}

template<typename T>
T CalculatorEngine<T>::evaluate(const std::string &expr, SymbolTable &symbolTable) {
    exprtk::parser<T> parser;
    exprtk::function_compositor<T> compositor;
    exprtk::symbol_table<T> symbols = compositor.symbol_table();

    std::vector<ScriptFunction<T>> funcs;

    for (auto &script : symbolTable.scripts) {
        ScriptFunction<T> func = ScriptFunction<T>(interpreter, script.second);
        funcs.template emplace_back(func);
        symbols.add_function(script.first, *(funcs.end() - 1));
    }

    for (auto &pair : symbolTable.functions) {
        auto func = pair.second;
        switch (func.arguments.size()) {
            case 0:
                compositor.add(typename exprtk::function_compositor<T>::function(func.name, func.expression));
                break;
            case 1:
                compositor.add(typename exprtk::function_compositor<T>::function(func.name, func.expression,
                                                                                 func.arguments[0]));
                break;
            case 2:
                compositor.add(
                        typename exprtk::function_compositor<T>::function(func.name, func.expression, func.arguments[0],
                                                                          func.arguments[1]));
                break;
            case 3:
                compositor.add(
                        typename exprtk::function_compositor<T>::function(func.name, func.expression, func.arguments[0],
                                                                          func.arguments[1],
                                                                          func.arguments[2]));
                break;
            case 4:
                compositor.add(
                        typename exprtk::function_compositor<T>::function(func.name, func.expression, func.arguments[0],
                                                                          func.arguments[1],
                                                                          func.arguments[2], func.arguments[3]));
                break;
            case 5:
                compositor.add(
                        typename exprtk::function_compositor<T>::function(func.name, func.expression, func.arguments[0],
                                                                          func.arguments[1],
                                                                          func.arguments[2], func.arguments[3],
                                                                          func.arguments[4]));
                break;
            default:
                throw std::runtime_error("Too many function arguments");
        }
    }

    for (auto &constant : symbolTable.constants) {
        symbols.add_constant(constant.first, constant.second);
    }

    for (auto &variable : symbolTable.variables) {
        symbols.add_variable(variable.first, variable.second);
    }

    exprtk::expression<T> expression;
    expression.register_symbol_table(symbols);

    if (parser.compile(expr, expression)) {
        return expression.value();
    } else {
        throw std::runtime_error(parser.error());
    }
}

template<typename T>
T CalculatorEngine<T>::evaluate(const std::string &expr) {
    exprtk::parser<T> parser;
    exprtk::expression<T> expression;

    if (parser.compile(expr, expression)) {
        return expression.value();
    } else {
        throw std::runtime_error("Failed to compile expression");
    }
}