#include "model.hpp"

#include "numberformat.hpp"

Model::Model() : listener(nullptr) {

}

void Model::setStateListener(StateListener &l) {
    this->listener = &l;
    l.onStateValueChanged(state.value);
    l.onStateHistoryChanged(state.history);
    l.onStateInputChanged(state.input);
    l.onStateSymbolTableChanged(state.symbolTable);
    l.onStateShowKeyPadChanged(state.showKeypad);
    l.onStateShowBitViewChanged(state.showBitView);
    l.onStateShowDockChanged(state.showDock);
    l.onStateHistoryLimitChanged(state.historyLimit);
    l.onStateDockPositionChanged(state.dockPosition);
    l.onStateDockSelectedTabChanged(state.dockSelectedTab);
    l.onStateWindowSizeChanged(state.windowSize);
    l.onStateCurrentVariableChanged(state.currentVariable);
    l.onStateCurrentConstantChanged(state.currentConstant);
    l.onStateCurrentFunctionChanged(state.currentFunction);
    l.onStateCurrentScriptChanged(state.currentScript);
}

void Model::evaluateInput() {
    state.value = engine.evaluate(state.input, state.symbolTable);
    state.history.emplace_back(std::pair<std::string, ValueType>(state.input, state.value));
    state.input = NumberFormat::toDecimal(state.value);
    listener->onStateValueChanged(state.value);
    listener->onStateHistoryChanged(state.history);
    listener->onStateInputChanged(state.input);
}

void Model::updateInput(const std::string &input) {
    state.input = input;
    listener->onStateInputChanged(state.input);
}

void Model::updateValue(ValueType value) {
    state.value = value;
    listener->onStateValueChanged(state.value);
}

void Model::addVariable(Variable var) {
    state.symbolTable.variables.push_back(var);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::removeVariable(int index) {
    state.symbolTable.variables.erase(state.symbolTable.variables.begin() + index);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::updateVariable(int index, Variable value) {
    state.symbolTable.variables.at(index) = value;
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::addConstant(Constant value) {
    state.symbolTable.constants.emplace_back(value);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::removeConstant(int index) {
    state.symbolTable.constants.erase(state.symbolTable.constants.begin() + index);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::updateConstant(int index, Constant value) {
    state.symbolTable.constants.at(index) = value;
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::addFunction(Function value) {
    state.symbolTable.functions.emplace_back(value);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::removeFunction(int index) {
    state.symbolTable.functions.erase(state.symbolTable.functions.begin() + index);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::updateFunction(int index, Function value) {
    state.symbolTable.functions.at(index) = value;
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::addScript(Script value) {
    state.symbolTable.scripts.emplace_back(value);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::removeScript(int index) {
    state.symbolTable.scripts.erase(state.symbolTable.scripts.begin() + index);
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::updateScript(int index, Script value) {
    state.symbolTable.scripts.at(index) = value;
    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::updateShowKeyPad(bool value) {
    state.showKeypad = value;
    listener->onStateShowKeyPadChanged(state.showKeypad);
}

void Model::updateShowBitView(bool value) {
    state.showBitView = value;
    listener->onStateShowBitViewChanged(state.showBitView);
}

void Model::updateShowDock(bool value) {
    state.showDock = value;
    listener->onStateShowDockChanged(state.showDock);
}

void Model::updateHistoryLimit(int limit) {
    state.historyLimit = limit;
    listener->onStateHistoryLimitChanged(state.historyLimit);
}

void Model::updateDockPosition(Qt::DockWidgetArea position) {
    state.dockPosition = position;
    listener->onStateDockPositionChanged(state.dockPosition);
}

void Model::updateDockSelectedTab(int tab) {
    state.dockSelectedTab = tab;
    listener->onStateDockSelectedTabChanged(state.dockSelectedTab);
}

void Model::updateWindowSize(QSize size) {
    state.windowSize = size;
    listener->onStateWindowSizeChanged(state.windowSize);
}

void Model::updateCurrentVariable(int currentVariable) {
    state.currentVariable = currentVariable;
    listener->onStateCurrentVariableChanged(state.currentVariable);
}

void Model::updateCurrentConstant(int currentConstant) {
    state.currentConstant = currentConstant;
    listener->onStateCurrentConstantChanged(state.currentConstant);
}

void Model::updateCurrentFunction(int currentFunction) {
    state.currentFunction = currentFunction;
    listener->onStateCurrentFunctionChanged(state.currentFunction);
}

void Model::updateCurrentScript(int currentScript) {
    state.currentScript = currentScript;
    listener->onStateCurrentScriptChanged(state.currentScript);
}

const State &Model::getState() {
    return state;
}
