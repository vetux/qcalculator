#include "model.hpp"

#include "numberformat.hpp"

Model::Model(StateListener listener)
        : listener(listener) {

}

void Model::evaluateInput() {
    state.value = engine.evaluate(state.input, state.symbolTable);
    state.history.emplace_back(std::pair<std::string, ValueType>(state.input, state.value));
    state.input = NumberFormat::toDecimal(state.value);
    listener(state);
}

void Model::updateInput(const std::string &input) {
    state.input = input;
    listener(state);
}

void Model::updateValue(ValueType value) {
    state.value = value;
    listener(state);
}

void Model::addVariable(Variable var) {
    state.symbolTable.variables.push_back(var);
    listener(state);
}

void Model::removeVariable(int index) {
    state.symbolTable.variables.erase(state.symbolTable.variables.begin() + index);
    listener(state);
}

void Model::updateVariable(int index, Variable value) {
    state.symbolTable.variables.at(index) = value;
    listener(state);
}

void Model::addConstant(Constant value) {
    state.symbolTable.constants.emplace_back(value);
    listener(state);
}

void Model::removeConstant(int index) {
    state.symbolTable.constants.erase(state.symbolTable.constants.begin() + index);
    listener(state);
}

void Model::updateConstant(int index, Constant value) {
    state.symbolTable.constants.at(index) = value;
    listener(state);
}

void Model::addFunction(Function value) {
    state.symbolTable.functions.emplace_back(value);
    listener(state);
}

void Model::removeFunction(int index) {
    state.symbolTable.functions.erase(state.symbolTable.functions.begin() + index);
    listener(state);
}

void Model::updateFunction(int index, Function value) {
    state.symbolTable.functions.at(index) = value;
    listener(state);
}

void Model::addScript(Script value) {
    state.symbolTable.scripts.emplace_back(value);
    listener(state);
}

void Model::removeScript(int index) {
    state.symbolTable.scripts.erase(state.symbolTable.scripts.begin() + index);
    listener(state);
}

void Model::updateScript(int index, Script value) {
    state.symbolTable.scripts.at(index) = value;
    listener(state);
}

void Model::updateShowKeyPad(bool value) {
    state.showKeypad = value;
    listener(state);
}

void Model::updateShowBitView(bool value) {
    state.showBitView = value;
    listener(state);
}

void Model::updateShowDock(bool value) {
    state.showDock = value;
    listener(state);
}

void Model::updateHistoryLimit(int limit) {
    state.historyLimit = limit;
    listener(state);
}

void Model::updateDockPosition(int position) {
    state.dockPosition = position;
    listener(state);
}

void Model::updateDockSelectedTab(int tab) {
    state.dockSelectedTab = tab;
    listener(state);
}

void Model::updateWindowSize(QSize size) {
    state.windowSize = size;
    listener(state);
}

void Model::updateCurrentVariable(int currentVariable) {
    state.currentVariable = currentVariable;
    listener(state);
}

void Model::updateCurrentConstant(int currentConstant) {
    state.currentConstant = currentConstant;
    listener(state);
}

void Model::updateCurrentFunction(int currentFunction) {
    state.currentFunction = currentFunction;
    listener(state);
}

void Model::updateCurrentScript(int currentScript) {
    state.currentScript = currentScript;
    listener(state);
}

const State &Model::getState() {
    return state;
}
