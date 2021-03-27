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

    applyCurrentValue();
    applySymbolTable();

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
    if (index == -1)
        currentVariable = UID_NULL;
    else
        currentVariable = variableMapping.at(index);
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

        if (currentVariable == UID_NULL)
            return;
        else
            convertedValue = symbolTable.getVariable(currentVariable).value;
    }

    if (currentVariable == UID_NULL) {
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
                currentVariable = UID_NULL;
                applyVariables();
            }
        } else {
            try {
                symbolTable.setVariable(currentVariable, {name, convertedValue});
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
            applyVariables();
        }
    }
}

void Presenter::onSelectedConstantChanged(int index) {
    if (index == -1)
        currentConstant = UID_NULL;
    else
        currentConstant = constantMapping.at(index);
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

        if (currentConstant == UID_NULL)
            return;
        else
            convertedValue = symbolTable.getConstant(currentConstant).value;
    }

    if (currentConstant == UID_NULL) {
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
                currentConstant = UID_NULL;
                applyConstants();
            }
        } else {
            try {
                symbolTable.setConstant(currentConstant, {name, convertedValue});
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
            applyConstants();
        }
    }
}

void Presenter::onSelectedFunctionChanged(int index) {
    if (index == -1)
        currentFunction = UID_NULL;
    else
        currentFunction = functionMapping.at(index);

    applyCurrentFunction();
}

void Presenter::onFunctionNameChanged(const std::string &value) {
    if (currentFunction == UID_NULL) {
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
                currentFunction = UID_NULL;
                applyFunctions();
            }
        } else {
            try {
                f.name = value;
                symbolTable.setFunction(currentFunction, f);
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
            applyFunctions();
        }
    }
}

void Presenter::onFunctionBodyChanged(const std::string &value) {
    assert(currentFunction != UID_NULL);
    Function f = symbolTable.getFunction(currentFunction);
    f.expression = value;
    symbolTable.setFunction(currentFunction, f);
}

void Presenter::onFunctionArgsChanged(const std::vector<std::string> &arguments) {
    assert(currentFunction != UID_NULL);
    Function f = symbolTable.getFunction(currentFunction);
    f.argumentNames = arguments;
    symbolTable.setFunction(currentFunction, f);
    view.setFunctionArgs(arguments);
}

void Presenter::onSelectedScriptChanged(int index) {
    if (index == -1)
        currentScript = UID_NULL;
    else
        currentScript = scriptMapping.at(index);

    applyCurrentScript();
}

void Presenter::onScriptNameChanged(const std::string &value) {
    if (currentScript == UID_NULL) {
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
                currentScript = UID_NULL;
                applyScripts();
            }
        } else {
            try {
                s.name = value;
                symbolTable.setScript(currentScript, s);
            }
            catch (const std::exception &e) {
                view.showWarningDialog("Error", e.what());
            }
            applyScripts();
        }
    }
}

void Presenter::onScriptBodyChanged(const std::string &value) {
    assert(currentScript != UID_NULL);
    Script s = symbolTable.getScript(currentScript);
    s.body = value;
    symbolTable.setScript(currentScript, s);
}

void Presenter::onScriptEnableArgsChanged(bool value) {
    assert(currentScript != UID_NULL);
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

            currentVariable = UID_NULL;
            currentConstant = UID_NULL;
            currentFunction = UID_NULL;
            currentScript = UID_NULL;

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

    //Sort by using map with names as keys
    std::map<std::string, UID> dataSorted;
    for (auto &v : symbolTable.getVariables()) {
        dataSorted[v.second.name] = v.first;
    }

    variableMapping.clear();

    //Update mapping with indices into the vector passed to the view
    std::vector<std::pair<std::string, std::string>> tmp;
    int currentIndex = -1;

    int i = 0;
    for (auto &v : dataSorted) {
        variableMapping[i] = v.second;

        if (v.second == currentVariable)
            currentIndex = i;

        Variable var = symbolTable.getVariable(v.second);
        tmp.emplace_back(std::pair<std::string, std::string>(var.name, toDecimal(var.value)));
        i++;
    }

    view.setVariableListView(tmp);
    view.setSelectedVariable(currentIndex);

    view.connectPresenter(*this);
}

void Presenter::applyConstants() {
    view.disconnectPresenter(*this);

    //Sort by using map with names as keys
    std::map<std::string, UID> dataSorted;
    for (auto &v : symbolTable.getConstants()) {
        dataSorted[v.second.name] = v.first;
    }

    constantMapping.clear();

    //Update mapping with indices into the vector passed to the view
    std::vector<std::pair<std::string, std::string>> tmp;
    int currentIndex = -1;

    int i = 0;
    for (auto &v : dataSorted) {
        constantMapping[i] = v.second;

        if (v.second == currentConstant)
            currentIndex = i;

        Constant var = symbolTable.getConstant(v.second);
        tmp.emplace_back(std::pair<std::string, std::string>(var.name, toDecimal(var.value)));
        i++;
    }

    view.setConstantsListView(tmp);
    view.setSelectedConstant(currentIndex);

    view.connectPresenter(*this);
}

void Presenter::applyFunctions() {
    view.disconnectPresenter(*this);

    //Sort by using map with names as keys
    std::map<std::string, UID> dataSorted;
    for (auto &v : symbolTable.getFunctions()) {
        dataSorted[v.second.name] = v.first;
    }

    functionMapping.clear();

    //Update mapping with indices into the vector passed to the view
    std::vector<std::string> tmp;
    int currentIndex = -1;

    int i = 0;
    for (auto &v : dataSorted) {
        functionMapping[i] = v.second;

        if (v.second == currentFunction)
            currentIndex = i;

        Function var = symbolTable.getFunction(v.second);
        tmp.emplace_back(var.name);
        i++;
    }

    view.setFunctionsListView(tmp);
    view.setSelectedFunction(currentIndex);

    if (currentFunction == UID_NULL) {
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

    int currentIndex = -1;
    for (auto &v : functionMapping) {
        if (v.second == currentFunction) {
            currentIndex = v.first;
            break;
        }
    }

    view.setSelectedFunction(currentIndex);

    if (currentFunction == UID_NULL) {
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

    //Sort by using map with names as keys
    std::map<std::string, UID> dataSorted;
    for (auto &v : symbolTable.getScripts()) {
        dataSorted[v.second.name] = v.first;
    }

    scriptMapping.clear();

    //Update mapping with indices into the vector passed to the view
    std::vector<std::string> tmp;
    int currentIndex = -1;

    int i = 0;
    for (auto &v : dataSorted) {
        scriptMapping[i] = v.second;

        if (v.second == currentScript)
            currentIndex = i;

        Script var = symbolTable.getScript(v.second);
        tmp.emplace_back(var.name);
        i++;
    }

    view.setScriptsListView(tmp);
    view.setSelectedScript(currentIndex);

    if (currentScript == UID_NULL) {
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

    int currentIndex = -1;
    for (auto &v : scriptMapping) {
        if (v.second == currentScript) {
            currentIndex = v.first;
            break;
        }
    }

    view.setSelectedScript(currentIndex);

    if (currentScript == UID_NULL) {
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
