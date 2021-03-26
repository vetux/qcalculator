#include "presenter.hpp"

#include <QStandardPaths>
#include <QTextStream>
#include <QDir>

#include "numberformat.hpp"
#include "serializer.hpp"
#include "fractiontest.hpp"

#define SETTINGS_FILENAME "/settings.json"

using namespace NumberFormat;
using namespace FractionTest;

QString getAppDir() {
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

Settings loadSettings() {
    QString fileName = getAppDir();
    fileName.append(SETTINGS_FILENAME);
    try {
        QFile file(fileName);
        if (file.exists()) {
            file.open(QFile::ReadOnly);

            QTextStream stream(&file);
            QString settingsContents = stream.readAll();
            stream.flush();

            file.close();

            return Serializer::deserializeSettings(settingsContents.toStdString());
        } else {
            return {};
        }
    }
    catch (const std::exception &e) {
        std::string error = "Failed to read settings file at ";
        error += fileName.toStdString();
        error += " Error: ";
        error += e.what();
        throw std::runtime_error(error);
    }
}

void saveSettings(const Settings &settings) {
    QString appDir = getAppDir();
    QString fileName = appDir;
    fileName.append(SETTINGS_FILENAME);
    try {
        if (!QDir(appDir).exists())
            QDir().mkpath(appDir);

        QFile file(fileName);

        file.open(QFile::WriteOnly | QFile::Truncate);

        QTextStream stream(&file);
        QString str = Serializer::serializeSettings(settings).c_str();
        stream << str;
        stream.flush();

        file.close();
    }
    catch (const std::exception &e) {
        std::string error = "Failed to write settings file at ";
        error += fileName.toStdString();
        error += " Error: ";
        error += e.what();
        throw std::runtime_error(error);
    }
}

SymbolTable loadSymbolTable(const std::string &filePath) {
    QFile file(filePath.c_str());
    if (!file.exists()) {
        std::string error = "File ";
        error += filePath;
        error += " not found.";
        throw std::runtime_error(error);
    }

    file.open(QFile::ReadOnly);

    QTextStream stream(&file);
    QString contents = stream.readAll();
    stream.flush();

    file.close();

    return Serializer::deserializeTable(contents.toStdString());
}

void saveSymbolTable(const std::string &filePath, const SymbolTable &symbolTable) {
    QFile file(filePath.c_str());
    QFileInfo info(file);

    //Failsafe, dont accept filepaths which point to an existing directory
    //as overwriting an existing directory with a file would not make sense in this case.
    if (info.isDir()) {
        std::string error = "File ";
        error += filePath;
        error += " is a directory.";
        throw std::runtime_error(error);
    }

    file.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream stream(&file);
    QString contents = Serializer::serializeTable(symbolTable).c_str();
    stream << contents;
    stream.flush();

    file.close();
}

Presenter::Presenter(View &view)
        : view(view), currentValue(0) {
}

void Presenter::init() {
    try {
        settings = loadSettings();
    }
    catch (const std::exception &e) {
        settings = {};
        std::string error = "Failed to load settings: ";
        error += e.what();
        view.showWarningDialog("Error", error);
    }
    view.setKeyPadVisibility(settings.showKeypad);
    view.setBitViewVisibility(settings.showBitView);
    view.setDockPosition(settings.dockPosition);
    view.setDockVisibility(settings.showDock);
    view.setActiveDockTab(settings.dockActiveTab);
    view.setWindowSize(settings.windowSize);

    view.connectPresenter(*this);
}

void Presenter::onWindowClose(const QCloseEvent &event) {
    try {
        saveSettings(settings);
    }
    catch (const std::exception &e) {
        std::string error = "Failed to save settings: ";
        error += e.what();
        view.showWarningDialog("Error", error);
    }
    view.quit();
}

void Presenter::onWindowResize(const QResizeEvent &event) {
    settings.windowSize = event.size();
}

void Presenter::onInputSubmit() {
    try {
        currentValue = calculatorEngine.evaluate(inputText, symbolTable);
        history.add(inputText, currentValue);
        inputText = toDecimal(currentValue);
    } catch (const std::exception &e) {
        std::string text = "Failed to evaluate { ";
        text += inputText;
        text += " } Error: ";
        text += e.what();
        view.showWarningDialog("Error", text);
    }

    view.setInputText(inputText);
    view.setHistory(history.getVectorWithDecimalStringValues());

    applyCurrentValue();

    applyVariables(); //Update in case of variable changes
}

void Presenter::onInputUpdate(const QString &value) {
    inputText = value.toStdString();
}

void Presenter::onDecimalSubmit(const QString &value) {
    try {
        currentValue = fromDecimal(value.toStdString());
        applyCurrentValue();
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value.toStdString();
        error += " as decimal.";

        view.showWarningDialog("Error", error);
    }
}

void Presenter::onHexSubmit(const QString &value) {
    try {
        currentValue = fromHex(value.toStdString());
        applyCurrentValue();
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value.toStdString();
        error += " as hex.";

        view.showWarningDialog("Error", error);
    }
}

void Presenter::onOctalSubmit(const QString &value) {
    try {
        currentValue = fromOctal(value.toStdString());
        applyCurrentValue();
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value.toStdString();
        error += " as octal.";

        view.showWarningDialog("Error", error);
    }
}

void Presenter::onBinarySubmit(const QString &value) {
    try {
        currentValue = fromBinary(value.toStdString());
        applyCurrentValue();
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value.toStdString();
        error += " as binary.";

        view.showWarningDialog("Error", error);
    }
}

void Presenter::onNumPadKeyPressed(NumPadKey key) {
    if (key == NumPadKey::KEY_EQUAL) {
        onInputSubmit();
    } else {
        inputText += convertNumPadKeyToString(key);
        view.setInputText(inputText);
    }
}

void Presenter::onBitViewKeyPressed(int bitIndex) {
    std::bitset<64> bits(currentValue);
    bits.flip(bitIndex);
    currentValue = bits.to_ulong();
    applyCurrentValue();
}

void Presenter::onSelectedVariableChanged(int index) {
    currentVariable = index;
}

void Presenter::onVariableChanged(const std::string &name, const std::string &value) {
    ValueType convertedValue;
    try {
        convertedValue = fromDecimal(value);
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value;
        error += " as decimal.";
        view.showWarningDialog("Error", error);

        if (currentVariable == -1)
            return;
        else
            convertedValue = symbolTable.getVariable(currentVariable).value;
    }

    if (currentVariable == -1) {
        if (name.empty()) {
            view.showWarningDialog("Error", "The variable name cannot be empty.");
        } else {
            try {
                Variable var = {name, convertedValue};
                symbolTable.addVariable(var);
                applyVariables();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    } else {
        if (name.empty()) {
            Variable v = symbolTable.getVariable(currentVariable);
            if (view.showQuestionDialog("Delete Variable", "Do you want to delete " +
                                                           v.name +
                                                           " ?")) {
                symbolTable.removeVariable(currentVariable);
                currentVariable = -1;
                applyVariables();
            }
        } else {
            try {
                symbolTable.setVariable(currentVariable, {name, convertedValue});
                applyVariables();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    }
}

void Presenter::onSelectedConstantChanged(int index) {
    currentConstant = index;
}

void Presenter::onConstantChanged(const std::string &name, const std::string &value) {
    ValueType convertedValue;
    try {
        convertedValue = fromDecimal(value);
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value;
        error += " as decimal.";
        view.showWarningDialog("Error", error);

        if (currentConstant == -1)
            return;
        else
            convertedValue = symbolTable.getConstant(currentConstant).value;
    }

    if (currentConstant == -1) {
        if (name.empty()) {
            view.showWarningDialog("Error", "The constant name cannot be empty.");
        } else {
            try {
                Constant con = {name, convertedValue};
                symbolTable.addConstant(con);
                applyConstants();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    } else {
        if (name.empty()) {
            Constant c = symbolTable.getConstant(currentConstant);
            if (view.showQuestionDialog("Delete Constant", "Do you want to delete " +
                                                           c.name +
                                                           " ?")) {
                symbolTable.removeConstant(currentConstant);
                currentConstant = -1;
                applyConstants();
            }
        } else {
            try {
                symbolTable.setConstant(currentConstant, {name, convertedValue});
                applyConstants();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    }
}

void Presenter::onSelectedFunctionChanged(int index) {
    currentFunction = index;
    applyCurrentFunction();
}

void Presenter::onFunctionNameChanged(const std::string &value) {
    if (currentFunction == -1) {
        if (value.empty()) {
            view.showWarningDialog("Error", "Function name cannot be empty.");
        } else {
            try {
                Function f;
                f.name = value;
                symbolTable.addFunction(f);
                applyFunctions();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    } else {
        Function f = symbolTable.getFunction(currentFunction);
        if (value.empty()) {
            if (view.showQuestionDialog("Delete Function", "Do you want to delete " +
                                                           f.name +
                                                           " ?")) {
                symbolTable.removeFunction(currentFunction);
                currentFunction = -1;
                applyFunctions();
            }
        } else {
            try {
                f.name = value;
                symbolTable.setFunction(currentFunction, f);
                applyFunctions();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    }
}

void Presenter::onFunctionBodyChanged(const std::string &value) {
    assert(currentFunction != -1);
    Function f = symbolTable.getFunction(currentFunction);
    f.expression = value;
    symbolTable.setFunction(currentFunction, f);
}

void Presenter::onFunctionArgsChanged(const std::vector<std::string> &arguments) {
    assert(currentFunction != -1);
    Function f = symbolTable.getFunction(currentFunction);
    f.argumentNames = arguments;
    symbolTable.setFunction(currentFunction, f);
    view.setFunctionArgs(arguments);
}

void Presenter::onSelectedScriptChanged(int index) {
    currentScript = index;
    applyCurrentScript();
}

void Presenter::onScriptNameChanged(const std::string &value) {
    if (currentScript == -1) {
        if (value.empty()) {
            view.showWarningDialog("Error", "Script name cannot be empty.");
        } else {
            try {
                Script s;
                s.name = value;
                symbolTable.addScript(s);
                applyScripts();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    } else {
        Script s = symbolTable.getScript(currentScript);
        if (value.empty()) {
            if (view.showQuestionDialog("Delete Script", "Do you want to delete " +
                                                         s.name +
                                                         " ?")) {
                symbolTable.removeScript(currentScript);
                currentScript = -1;
                applyScripts();
            }
        } else {
            try {
                s.name = value;
                symbolTable.setScript(currentScript, s);
                applyScripts();
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
        }
    }
}

void Presenter::onScriptBodyChanged(const std::string &value) {
    assert(currentScript != -1);
    Script s = symbolTable.getScript(currentScript);
    s.body = value;
    symbolTable.setScript(currentScript, s);
}

void Presenter::onScriptEnableArgsChanged(bool value) {
    assert(currentScript != -1);
    Script s = symbolTable.getScript(currentScript);
    s.enableArguments = value;
    symbolTable.setScript(currentScript, s);
}

void Presenter::onActionExit() {
    try {
        saveSettings(settings);
    }
    catch (const std::exception &e) {
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
    settings.showKeypad = show;
    view.setKeyPadVisibility(show);
}

void Presenter::onActionShowBitView(bool show) {
    settings.showBitView = show;
    view.setBitViewVisibility(show);
}

void Presenter::onActionShowDock(bool show) {
    settings.showDock = show;
    view.setDockVisibility(show);
}

void Presenter::onActionImportSymbolTable() {
    std::string filepath;
    if (view.showFileChooserDialog("Import symbol table", true, filepath)) {
        try {
            symbolTable = loadSymbolTable(filepath);

            currentVariable = -1;
            currentConstant = -1;
            currentFunction = -1;
            currentScript = -1;

            applySymbolTable();

            view.showInfoDialog("Import successful", "Successfully imported symbol table from " + filepath);
        }
        catch (const std::exception &e) {
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
            saveSymbolTable(filepath, symbolTable);
            view.showInfoDialog("Export successful", "Successfully exported symbol table to " + filepath);
        }
        catch (const std::exception &e) {
            std::string error = "Failed to export symbol table to ";
            error += filepath;
            error += " Error: ";
            error += e.what();
            view.showWarningDialog("Export failed", error);
        }
    }
}

void Presenter::onDockTabChanged(int tabIndex) {
    settings.dockActiveTab = tabIndex;
}

void Presenter::onDockVisibilityChanged(bool visible) {
    settings.showDock = visible;
}

bool isValidArea(Qt::DockWidgetArea area) {
    switch (area) {
        case Qt::DockWidgetArea::LeftDockWidgetArea:
        case Qt::DockWidgetArea::TopDockWidgetArea:
        case Qt::DockWidgetArea::RightDockWidgetArea:
        case Qt::DockWidgetArea::BottomDockWidgetArea:
            return true;
        default:
            return false;
    }
}

void Presenter::onDockPositionChanged(Qt::DockWidgetArea area) {
    if (isValidArea(area))
        settings.dockPosition = area;
}

void Presenter::applyCurrentValue() {
    view.setValueText(toDecimal(currentValue));
    view.setDecimalText(toDecimal(currentValue));
    view.setHexText(toHex(currentValue));
    view.setOctalText(toOctal(currentValue));
    view.setBinaryText(toBinary(currentValue));

    if (currentValue < 0 || hasFraction(currentValue)) {
        view.setBitViewEnabled(false);
        view.setBitViewContents(std::bitset<64>(0));
    } else {
        view.setBitViewEnabled(true);
        view.setBitViewContents(std::bitset<64>(currentValue));
    }
}

void Presenter::applySymbolTable() {
    applyVariables();
    applyConstants();
    applyFunctions();
    applyScripts();
}

void Presenter::applyVariables() {
    view.disconnectPresenter(*this);

    std::vector<std::pair<std::string, std::string>> tmp;
    for (auto &v : symbolTable.getVariables()) {
        tmp.emplace_back(std::pair<std::string, std::string>(v.name, toDecimal(v.value)));
    }
    view.setVariableListView(tmp);
    view.setSelectedVariable(currentVariable);

    view.connectPresenter(*this);
}

void Presenter::applyConstants() {
    view.disconnectPresenter(*this);

    std::vector<std::pair<std::string, std::string>> tmp;
    for (auto &v : symbolTable.getConstants()) {
        tmp.emplace_back(std::pair<std::string, std::string>(v.name, toDecimal(v.value)));
    }
    view.setConstantsListView(tmp);
    view.setSelectedConstant(currentConstant);

    view.connectPresenter(*this);
}

void Presenter::applyFunctions() {
    view.disconnectPresenter(*this);

    std::vector<std::string> tmp;
    for (auto &v : symbolTable.getFunctions()) {
        tmp.emplace_back(v.name);
    }
    view.setFunctionsListView(tmp);

    view.setSelectedFunction(currentFunction);

    if (currentFunction == -1) {
        view.setFunctionBodyEnabled(false);
        view.setFunctionArgsSpinBoxEnabled(false);
        view.setFunctionBody("");
        view.setFunctionArgs({});
    } else {
        Function func = symbolTable.getFunction(currentFunction);

        view.setFunctionBodyEnabled(true);
        view.setFunctionArgsSpinBoxEnabled(true);
        view.setFunctionBody(func.expression);
        view.setFunctionArgs(func.argumentNames);
    }

    view.connectPresenter(*this);
}

void Presenter::applyCurrentFunction() {
    view.disconnectPresenter(*this);

    view.setSelectedFunction(currentFunction);

    if (currentFunction == -1) {
        view.setFunctionBodyEnabled(false);
        view.setFunctionArgsSpinBoxEnabled(false);
        view.setFunctionBody("");
        view.setFunctionArgs({});
    } else {
        Function func = symbolTable.getFunction(currentFunction);

        view.setFunctionBodyEnabled(true);
        view.setFunctionArgsSpinBoxEnabled(true);
        view.setFunctionBody(func.expression);
        view.setFunctionArgs(func.argumentNames);
    }

    view.connectPresenter(*this);
}

void Presenter::applyScripts() {
    view.disconnectPresenter(*this);

    std::vector<std::string> tmp;
    for (auto &v : symbolTable.getScripts()) {
        tmp.emplace_back(v.name);
    }
    view.setScriptsListView(tmp);

    view.setSelectedScript(currentScript);

    if (currentScript == -1) {
        view.setScriptEnableArgsEnabled(false);
        view.setScriptBodyEnabled(false);
        view.setScriptEnableArgs(false);
        view.setScriptBody("");
    } else {
        Script s = symbolTable.getScript(currentScript);
        view.setScriptEnableArgsEnabled(true);
        view.setScriptBodyEnabled(true);
        view.setScriptEnableArgs(s.enableArguments);
        view.setScriptBody(s.body);
    }

    view.connectPresenter(*this);
}

void Presenter::applyCurrentScript() {
    view.disconnectPresenter(*this);

    view.setSelectedScript(currentScript);

    if (currentScript == -1) {
        view.setScriptEnableArgsEnabled(false);
        view.setScriptBodyEnabled(false);
        view.setScriptEnableArgs(false);
        view.setScriptBody("");
    } else {
        Script s = symbolTable.getScript(currentScript);
        view.setScriptEnableArgsEnabled(true);
        view.setScriptBodyEnabled(true);
        view.setScriptEnableArgs(s.enableArguments);
        view.setScriptBody(s.body);
    }

    view.connectPresenter(*this);
}
