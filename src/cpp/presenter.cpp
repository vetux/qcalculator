#include "presenter.hpp"

#include "numberformat.hpp"

Presenter::Presenter(Model &model, View &view)
        : model(model), view(view) {
}

void Presenter::init() {
    view.connectPresenter(*this);
}

void Presenter::onStateChange(const State &state) {
    view.disconnectPresenter(*this);

    view.setInputText(state.input);
    view.setDecimalText(NumberFormat::toDecimal(state.value));
    view.setHexText(NumberFormat::toHex(state.value));
    view.setOctalText(NumberFormat::toOctal(state.value));
    view.setBinaryText(NumberFormat::toBinary(state.value));

    view.setKeyPadVisibility(state.showKeypad);
    view.setBitViewVisibility(state.showBitView);
    view.setDockVisibility(state.showDock);

    view.setActiveDockTab(state.dockSelectedTab);

    view.setWindowSize(state.windowSize);

    std::vector<std::pair<std::string, std::string>> tmp;
    for (auto &pair : state.history) {
        tmp.emplace_back(std::pair<std::string, std::string>(pair.first, NumberFormat::toDecimal(pair.second)));
    }
    view.setHistory(tmp);

    tmp.clear();
    for (auto &v : state.symbolTable.variables) {
        tmp.emplace_back(std::pair<std::string, std::string>(v.name, NumberFormat::toDecimal(v.value)));
    }
    view.setVariableListView(tmp);
    view.setActiveVariable(state.currentVariable);

    tmp.clear();
    for (auto &v : state.symbolTable.constants) {
        tmp.emplace_back(std::pair<std::string, std::string>(v.name, NumberFormat::toDecimal(v.value)));
    }
    view.setConstantsListView(tmp);
    view.setActiveConstant(state.currentConstant);

    std::vector<std::string> names;
    for (auto &v : state.symbolTable.functions) {
        names.emplace_back(v.name);
    }
    view.setFunctionsListView(names);
    view.setActiveFunction(state.currentFunction);

    if (state.currentFunction >= 0) {
        view.setFunctionArgs(state.symbolTable.functions.at(state.currentFunction).argumentNames);
        view.setFunctionBody(state.symbolTable.functions.at(state.currentFunction).expression);
        view.setFunctionArgsEnabled(true);
        view.setFunctionBodyEnabled(true);
    } else {
        view.setFunctionArgs({});
        view.setFunctionBody("");
        view.setFunctionArgsEnabled(false);
        view.setFunctionBodyEnabled(false);
    }

    names.clear();
    for (auto &v : state.symbolTable.scripts) {
        names.emplace_back(v.name);
    }
    view.setScriptsListView(names);
    view.setActiveScript(state.currentScript);

    if (state.currentScript >= 0) {
        view.setScriptBody(state.symbolTable.scripts.at(state.currentScript).body);
        view.setScriptBodyEnabled(true);
    } else {
        view.setScriptBody("");
        view.setScriptBodyEnabled(false);
    }

    view.connectPresenter(*this);
}

void Presenter::onWindowClose(QCloseEvent *event) {
    view.quit();
}

void Presenter::onWindowResize(QResizeEvent *event) {
    model.updateWindowSize(event->size());
}

void Presenter::onInputSubmit() {
    model.evaluateInput();
}

void Presenter::onInputUpdate(const QString &value) {
    model.updateInput(value.toStdString());
}

void Presenter::onDecimalSubmit(QString value) {
    model.updateValue(NumberFormat::fromDecimal(value.toStdString()));
}

void Presenter::onHexSubmit(QString value) {
    model.updateValue(NumberFormat::fromHex(value.toStdString()));
}

void Presenter::onOctalSubmit(QString value) {
    model.updateValue(NumberFormat::fromOctal(value.toStdString()));
}

void Presenter::onBinarySubmit(QString value) {
    model.updateValue(NumberFormat::fromBinary(value.toStdString()));
}

void Presenter::onNumPadKeyPressed(NumPadKey key) {
    //TODO: Add key to input
}

void Presenter::onBitViewKeyPressed(int bitIndex) {
    //TODO: Get bitset from value and calculate new value based on bit toggle
}

void Presenter::onDockVisibilityChanged(bool visible) {
    model.updateShowDock(visible);
}

void Presenter::onCurrentScriptChanged(int index) {
    model.updateCurrentScript(index);
}

void Presenter::onScriptNameChanged(std::string value) {
    const State &state = model.getState();
    Script s = state.symbolTable.scripts.at(state.currentScript);
    s.name = value;
    model.updateScript(state.currentScript, s);
}

void Presenter::onScriptBodyChanged(std::string value) {
    const State &state = model.getState();
    Script s = state.symbolTable.scripts.at(state.currentScript);
    s.body = value;
    model.updateScript(state.currentScript, s);
}

void Presenter::onCurrentFunctionChanged(int index) {
    model.updateCurrentFunction(index);
}

void Presenter::onFunctionNameChanged(std::string value) {
    const State &state = model.getState();
    Function f = state.symbolTable.functions.at(state.currentFunction);
    f.name = value;
    model.updateFunction(state.currentScript, f);
}

void Presenter::onFunctionBodyChanged(std::string value) {
    const State &state = model.getState();
    Function f = state.symbolTable.functions.at(state.currentFunction);
    f.expression = value;
    model.updateFunction(state.currentScript, f);
}

void Presenter::onFunctionArgsChanged(std::vector<std::string> arguments) {
    const State &state = model.getState();
    Function f = state.symbolTable.functions.at(state.currentFunction);
    f.argumentNames = arguments;
    model.updateFunction(state.currentScript, f);
}

void Presenter::onCurrentVariableChanged(int index) {
    model.updateCurrentVariable(index);
}

void Presenter::onVariableChanged(std::string name, std::string value) {
    const State &state = model.getState();
    model.updateVariable(state.currentVariable, {name, NumberFormat::fromDecimal(value)});
}

void Presenter::onCurrentConstantChanged(int index) {
    model.updateCurrentConstant(index);
}

void Presenter::onConstantChanged(std::string name, std::string value) {
    const State &state = model.getState();
    model.updateConstant(state.currentConstant, {name, NumberFormat::fromDecimal(value)});
}

void Presenter::onActionExit() {
    view.quit();
}

void Presenter::onActionAbout() {
    view.launchAboutDialog();
}

void Presenter::onActionSettings() {
    view.launchSettingsDialog();
}

void Presenter::onActionShowKeyPad(bool show) {
    model.updateShowKeyPad(show);
}

void Presenter::onActionShowBitView(bool show) {
    model.updateShowBitView(show);
}

void Presenter::onActionShowDock(bool show) {
    model.updateShowDock(show);
}

void Presenter::onDockTabChanged(int tabIndex) {
    model.updateDockSelectedTab(tabIndex);
}
