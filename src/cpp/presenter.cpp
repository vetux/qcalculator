#include "presenter.hpp"

#include "numberformat.hpp"
#include "serializer.hpp"
#include "fractiontest.hpp"

Presenter::Presenter(Model &model, View &view)
        : model(model), view(view) {
}

void Presenter::init() {
    try {
        model.loadSettings();
    }
    catch (std::exception &e) {
        std::string error = "Failed to load settings: ";
        error += e.what();
        view.showWarningDialog("Error", error);
    }
}

void Presenter::onStateValueChanged(ValueType value) {
    view.disconnectPresenter(*this);

    view.setValueText(NumberFormat::toDecimal(value));

    if (hasFraction(value) || value < 0) {
        view.setNumericSystemsEnabled(false);
        view.setDecimalText("");
        view.setHexText("");
        view.setOctalText("");
        view.setBinaryText("");

        view.setBitViewEnabled(false);
        view.setBitViewContents(std::bitset<64>(0));
    } else {
        view.setNumericSystemsEnabled(true);
        view.setDecimalText(NumberFormat::toDecimal(value));
        view.setHexText(NumberFormat::toHex(value));
        view.setOctalText(NumberFormat::toOctal(value));
        view.setBinaryText(NumberFormat::toBinary(value));

        view.setBitViewEnabled(true);
        view.setBitViewContents(std::bitset<64>(static_cast<unsigned long>(value)));
    }

    view.connectPresenter(*this);
}

void Presenter::onStateHistoryChanged(std::vector<std::pair<std::string, ValueType>> value) {
    view.disconnectPresenter(*this);

    std::vector<std::pair<std::string, std::string>> tmp;
    for (auto &pair : value) {
        tmp.emplace_back(std::pair<std::string, std::string>(pair.first, NumberFormat::toDecimal(pair.second)));
    }
    view.setHistory(tmp);

    view.connectPresenter(*this);
}

void Presenter::onStateInputChanged(std::string value) {
    view.disconnectPresenter(*this);

    view.setInputText(value);

    view.connectPresenter(*this);
}

void Presenter::onStateSymbolTableChanged(SymbolTable value) {
    view.disconnectPresenter(*this);

    std::vector<std::pair<std::string, std::string>> tmp;
    for (auto &v : value.variables) {
        tmp.emplace_back(std::pair<std::string, std::string>(v.name, NumberFormat::toDecimal(v.value)));
    }
    view.setVariableListView(tmp);

    tmp.clear();
    for (auto &v : value.constants) {
        tmp.emplace_back(std::pair<std::string, std::string>(v.name, NumberFormat::toDecimal(v.value)));
    }
    view.setConstantsListView(tmp);

    std::vector<std::string> names;
    for (auto &v : value.functions) {
        names.emplace_back(v.name);
    }
    view.setFunctionsListView(names);
    view.setSelectedFunction(model.getState().currentFunction);

    names.clear();
    for (auto &v : value.scripts) {
        names.emplace_back(v.name);
    }
    view.setScriptsListView(names);
    view.setSelectedScript(model.getState().currentScript);

    view.connectPresenter(*this);
}

void Presenter::onStateShowKeyPadChanged(bool value) {
    view.disconnectPresenter(*this);

    view.setKeyPadVisibility(value);

    view.connectPresenter(*this);
}

void Presenter::onStateShowBitViewChanged(bool value) {
    view.disconnectPresenter(*this);

    view.setBitViewVisibility(value);

    view.connectPresenter(*this);
}

void Presenter::onStateShowDockChanged(bool value) {
    view.disconnectPresenter(*this);

    view.setDockVisibility(value);

    view.connectPresenter(*this);
}

void Presenter::onStateHistoryLimitChanged(int value) {
    view.disconnectPresenter(*this);


    view.connectPresenter(*this);
}

void Presenter::onStateDockPositionChanged(Qt::DockWidgetArea value) {
    view.disconnectPresenter(*this);

    view.setDockPosition(value);

    view.connectPresenter(*this);
}

void Presenter::onStateDockSelectedTabChanged(int value) {
    view.disconnectPresenter(*this);

    view.setActiveDockTab(value);

    view.connectPresenter(*this);
}

void Presenter::onStateWindowSizeChanged(QSize value) {
    view.disconnectPresenter(*this);

    view.setWindowSize(value);

    view.connectPresenter(*this);
}

void Presenter::onStateCurrentVariableChanged(int value) {
    view.disconnectPresenter(*this);

    view.setSelectedVariable(value);

    view.connectPresenter(*this);
}

void Presenter::onStateCurrentConstantChanged(int value) {
    view.disconnectPresenter(*this);

    view.setSelectedConstant(value);

    view.connectPresenter(*this);
}

void Presenter::onStateCurrentFunctionChanged(int value) {
    view.disconnectPresenter(*this);

    view.setSelectedFunction(value);

    if (value >= 0) {
        view.setFunctionArgs(model.getState().symbolTable.functions.at(value).argumentNames);
        view.setFunctionBody(model.getState().symbolTable.functions.at(value).expression);
        view.setFunctionArgsSpinBoxEnabled(true);
        view.setFunctionBodyEnabled(true);
    } else {
        view.setFunctionArgs({});
        view.setFunctionBody("");
        view.setFunctionArgsSpinBoxEnabled(false);
        view.setFunctionBodyEnabled(false);
    }

    view.connectPresenter(*this);
}

void Presenter::onStateCurrentScriptChanged(int value) {
    view.disconnectPresenter(*this);

    view.setSelectedScript(value);

    if (value >= 0) {
        Script s = model.getState().symbolTable.scripts.at(value);
        view.setScriptBody(s.body);
        view.setScriptBodyEnabled(true);
        view.setScriptEnableArgs(s.enableArguments);
        view.setScriptEnableArgsEnabled(true);
    } else {
        view.setScriptBody("");
        view.setScriptBodyEnabled(false);
        view.setScriptEnableArgs(false);
        view.setScriptEnableArgsEnabled(false);
    }

    view.connectPresenter(*this);
}

void Presenter::onWindowClose(QCloseEvent *event) {
    try {
        model.saveSettings();
    }
    catch (std::exception &e) {
        std::string error = "Failed to save settings: ";
        error += e.what();
        view.showWarningDialog("Error", error);
    }
    view.quit();
}

void Presenter::onWindowResize(QResizeEvent *event) {
    model.updateWindowSize(event->size());
}

void Presenter::onInputSubmit() {
    try {
        model.evaluateInput();
    } catch (std::exception &e) {
        std::string text = "Failed to evaluate { ";
        text += model.getState().input;
        text += " } Error: ";
        text += e.what();
        view.showWarningDialog("Error", text);
    }
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
    if (key == NumPadKey::KEY_EQUAL) {
        onInputSubmit();
    } else {
        model.updateInput(model.getState().input + convertNumPadKeyToString(key));
    }
}

void Presenter::onBitViewKeyPressed(int bitIndex) {
    std::bitset<64> bits(model.getState().value);
    bits.flip(bitIndex);
    model.updateValue(bits.to_ulong());
}

void Presenter::onSelectedVariableChanged(int index) {
    model.updateCurrentVariable(index);
}

void Presenter::onVariableChanged(std::string name, std::string value) {
    const State &state = model.getState();
    if (state.currentVariable == -1) {
        if (name.empty()) {
            view.showWarningDialog("Error", "The variable name cannot be empty.");
        } else {
            Variable var = {name, NumberFormat::fromDecimal(value)};
            model.addVariable(var);
        }
    } else {
        if (name.empty()) {
            Variable v = state.symbolTable.variables.at(state.currentVariable);
            if (view.showQuestionDialog("Delete Variable", "Do you want to delete " +
                                                           v.name +
                                                           " ?")) {
                model.removeVariable(state.currentVariable);
            } else {
                model.updateVariable(state.currentVariable, v);
            }
        } else {
            model.updateVariable(state.currentVariable, {name, NumberFormat::fromDecimal(value)});
        }
    }
}

void Presenter::onSelectedConstantChanged(int index) {
    model.updateCurrentConstant(index);
}

void Presenter::onConstantChanged(std::string name, std::string value) {
    const State &state = model.getState();
    if (state.currentConstant == -1) {
        if (name.empty()) {
            view.showWarningDialog("Error", "The constant name cannot be empty.");
        } else {
            Constant con = {name, NumberFormat::fromDecimal(value)};
            model.addConstant(con);
        }
    } else {
        if (name.empty()) {
            Constant c = state.symbolTable.constants.at(state.currentConstant);
            if (view.showQuestionDialog("Delete Constant", "Do you want to delete " +
                                                           c.name +
                                                           " ?")) {
                model.removeConstant(state.currentConstant);
            } else {
                model.updateConstant(state.currentConstant, c);
            }
        } else {
            model.updateConstant(state.currentConstant, {name, NumberFormat::fromDecimal(value)});
        }
    }
}

void Presenter::onSelectedFunctionChanged(int index) {
    model.updateCurrentFunction(index);
}

void Presenter::onFunctionNameChanged(std::string value) {
    const State &state = model.getState();
    if (state.currentFunction == -1) {
        if (value.empty()) {
            view.showWarningDialog("Error", "Function name cannot be empty.");
        } else {
            Function f;
            f.name = value;
            model.addFunction(f);
        }
    } else {
        Function f = state.symbolTable.functions.at(state.currentFunction);
        if (value.empty()) {
            if (view.showQuestionDialog("Delete Function", "Do you want to delete " +
                                                           f.name +
                                                           " ?")) {
                model.removeFunction(state.currentFunction);
            } else {
                model.updateFunction(state.currentFunction, f);
            }
        } else {
            f.name = value;
            model.updateFunction(state.currentFunction, f);
        }
    }
}

void Presenter::onFunctionBodyChanged(std::string value) {
    const State &state = model.getState();
    assert(state.currentFunction != -1);
    Function f = state.symbolTable.functions.at(state.currentFunction);
    f.expression = value;
    model.updateFunction(state.currentFunction, f);
}

void Presenter::onFunctionArgsChanged(std::vector<std::string> arguments) {
    const State &state = model.getState();
    assert(state.currentFunction != -1);
    Function f = state.symbolTable.functions.at(state.currentFunction);
    f.argumentNames = std::move(arguments);
    model.updateFunction(state.currentFunction, f);
    model.updateCurrentFunction(state.currentFunction);
}

void Presenter::onSelectedScriptChanged(int index) {
    model.updateCurrentScript(index);
}

void Presenter::onScriptNameChanged(std::string value) {
    const State &state = model.getState();
    if (state.currentScript == -1) {
        if (value.empty()) {
            view.showWarningDialog("Error", "Script name cannot be empty.");
        } else {
            Script s;
            s.name = value;
            model.addScript(s);
        }
    } else {
        Script s = state.symbolTable.scripts.at(state.currentScript);
        if (value.empty()) {
            if (view.showQuestionDialog("Delete Script", "Do you want to delete " +
                                                         s.name +
                                                         " ?")) {
                model.removeScript(state.currentScript);
            } else {
                model.updateScript(state.currentScript, s);
            }
        } else {
            s.name = value;
            model.updateScript(state.currentScript, s);
        }
    }
}

void Presenter::onScriptBodyChanged(std::string value) {
    const State &state = model.getState();
    assert(state.currentScript != -1);
    Script s = state.symbolTable.scripts.at(state.currentScript);
    s.body = value;
    model.updateScript(state.currentScript, s);
}

void Presenter::onScriptEnableArgsChanged(bool value) {
    const State &state = model.getState();
    assert(state.currentScript != -1);
    Script s = state.symbolTable.scripts.at(state.currentScript);
    s.enableArguments = value;
    model.updateScript(state.currentScript, s);
}

void Presenter::onActionExit() {
    try {
        model.saveSettings();
    }
    catch (std::exception &e) {
        std::string error = "Failed to save settings: ";
        error += e.what();
        view.showWarningDialog("Error", error);
    }
    view.quit();
}

void Presenter::onActionAbout() {
    view.showAboutDialog();
}

void Presenter::onActionSettings() {
    view.showSettingsDialog();
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

void Presenter::onActionImportSymbolTable() {
    std::string filepath;
    if (view.showFileChooserDialog("Import symbol table", true, filepath)) {
        try {
            model.importSymbolTable(filepath);
            view.showInfoDialog("Import successful", "Successfully imported symbol table from " + filepath);
        }
        catch (std::exception &e) {
            std::string error = "Failed to import symbol table at ";
            error += filepath;
            error += " Error: ";
            error += e.what();
            view.showWarningDialog("Import failed", error);
        }
    }
}

void Presenter::onActionExportSymbolTable() {
    std::string filepath;
    if (view.showFileChooserDialog("Export symbol table", false, filepath)) {
        try {
            model.exportSymbolTable(filepath);
            view.showInfoDialog("Export successful", "Successfully exported symbol table to " + filepath);
        }
        catch (std::exception &e) {
            std::string error = "Failed to export symbol table to ";
            error += filepath;
            error += " Error: ";
            error += e.what();
            view.showWarningDialog("Export failed", error);
        }
    }
}

void Presenter::onDockTabChanged(int tabIndex) {
    model.updateDockSelectedTab(tabIndex);
}

void Presenter::onDockVisibilityChanged(bool visible) {
    model.updateShowDock(visible);
}