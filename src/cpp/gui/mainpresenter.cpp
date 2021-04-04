#include <QStandardPaths>
#include <QTextStream>
#include <QDir>
#include <QCoreApplication>

#include "serializer.hpp"
#include "fractiontest.hpp"
#include "numberformat.hpp"
#include "pyutil.hpp"
#include "io.hpp"
#include "addonhelper.hpp"

#include "gui/mainpresenter.hpp"

#include "pymodule/pysymboltable.hpp"
#include "pymodule/presentermodule.hpp"
#include "pymodule/exprtkmodule.hpp"

#define SETTINGS_FILENAME "/settings.json"

using namespace NumberFormat;
using namespace FractionTest;

QString getAddonModulesDirectory() {
    return QCoreApplication::applicationDirPath().append("/addon");
}

QString getAppDataDirectory() {
    QString dirPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    if (QDir(dirPath).exists())
        QDir().mkpath(dirPath);

    return dirPath;
}

MainPresenter::MainPresenter(MainView &view)
        : view(view), currentValue(0), addonManager(*this) {}

void MainPresenter::init() {
    PresenterModule::initialize(*this);
    ExprtkModule::initialize();
    PyUtil::initializePython();
    PyUtil::addModuleDirectory(getAddonModulesDirectory().toStdString());
    PyUtil::addModuleDirectory(QCoreApplication::applicationDirPath().append("/system").toStdString());

    try {
        settings = Serializer::deserializeSettings(
                IO::fileReadAllText(getAppDataDirectory().append(SETTINGS_FILENAME).toStdString()));
    }
    catch (const std::exception &e) {
        settings = {};
        std::string error = "Failed to load settings: ";
        error += e.what();
        view.showWarningDialog("Error", error);
    }

    applySettings();
    applyCurrentValue();
    applySymbolTable();

    addonManager.setActiveAddons(settings.enabledAddonModules);
}

void MainPresenter::onWindowClose(const QCloseEvent &event) {
    try {
        IO::fileWriteAllText(getAppDataDirectory().append(SETTINGS_FILENAME).toStdString(),
                             Serializer::serializeSettings(settings));
    }
    catch (const std::exception &e) {
        std::string error = "Failed to save settings: ";
        error += e.what();
        view.showWarningDialog("Error", error);
    }

    addonManager.setActiveAddons({});

    // Because an addon can have a pending deleteLater waiting to be processed
    // and qt does not offer a way to force deleteLater events to be processed
    // calling finalizePython here results in a segfault.
    // PyUtil::finalizePython();
}

void MainPresenter::onWindowResize(const QResizeEvent &event) {
    settings.windowSize = event.size();
}

void MainPresenter::onInputSubmit() {
    try {
        currentValue = ExpressionParser::evaluate(inputText, symbolTable);
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

void MainPresenter::onInputUpdate(const QString &value) {
    inputText = value.toStdString();
    autoUpdateValueFromInputText();
}

void MainPresenter::onDecimalSubmit(const QString &value) {
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

void MainPresenter::onHexSubmit(const QString &value) {
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

void MainPresenter::onOctalSubmit(const QString &value) {
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

void MainPresenter::onBinarySubmit(const QString &value) {
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

void MainPresenter::onNumPadKeyPressed(NumPadKey key) {
    if (key == NumPadKey::KEY_EQUAL) {
        onInputSubmit();
    } else {
        inputText += convertNumPadKeyToString(key);
        view.setInputText(inputText);
        autoUpdateValueFromInputText();
    }
}

void MainPresenter::onBitViewKeyPressed(int bitIndex) {
    std::bitset<64> bits(currentValue);
    bits.flip(bitIndex);
    currentValue = bits.to_ulong();
    applyCurrentValue();
    inputText = toDecimal(currentValue);
    view.setInputText(inputText);
}

void MainPresenter::onSelectedVariableChanged(int index) {
    if (index == -1)
        currentVariable.clear();
    else
        currentVariable = variableMapping.at(index);
}

void MainPresenter::onVariableChanged(const std::string &name, const std::string &value) {
    ArithmeticType convertedValue;
    try {
        convertedValue = fromDecimal(value);
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value;
        error += " as decimal.";
        view.showWarningDialog("Error", error);

        if (currentVariable.empty())
            return;
        else
            convertedValue = symbolTable.getVariables().at(currentVariable);
    }

    if (currentVariable.empty()) {
        //Create
        if (name.empty()) {
            view.showWarningDialog("Error", "The variable name cannot be empty.");
        } else {
            if (symbolTable.hasVariable(name)) {
                view.showWarningDialog("Error",
                                       "Variable with the name " + name + " already exists.");
            } else if (symbolTable.hasConstant(name)) {
                view.showWarningDialog("Error",
                                       "Constant with the name " + name + " already exists.");
            } else if (symbolTable.hasFunction(name)) {
                view.showWarningDialog("Error",
                                       "Function with the name " + name + " already exists.");
            } else if (symbolTable.hasScript(name)) {
                view.showWarningDialog("Error",
                                       "Script with the name " + name + " already exists.");
            } else {
                symbolTable.setVariable(name, convertedValue);
                applyVariables();
            }
        }
    } else {
        if (name.empty()) {
            //Delete
            if (view.showQuestionDialog("Delete Variable",
                                        "Do you want to delete " + currentVariable + " ?")) {
                symbolTable.remove(currentVariable);
                currentVariable.clear();
                applyVariables();
            }
        } else {
            if (name != currentVariable) {
                //Update name
                if (symbolTable.hasVariable(name)) {
                    view.showWarningDialog("Error",
                                           "Variable with the name " + name +
                                           " already exists.");
                } else if (symbolTable.hasConstant(name)) {
                    view.showWarningDialog("Error",
                                           "Constant with the name " + name +
                                           " already exists.");
                } else if (symbolTable.hasFunction(name)) {
                    view.showWarningDialog("Error",
                                           "Function with the name " + name +
                                           " already exists.");
                } else if (symbolTable.hasScript(name)) {
                    view.showWarningDialog("Error",
                                           "Script with the name " + name +
                                           " already exists.");
                } else {
                    symbolTable.remove(currentVariable);
                    symbolTable.setVariable(name, convertedValue);
                    currentVariable = name;
                }
            } else {
                //Update value
                symbolTable.setVariable(currentVariable, convertedValue);
            }
            applyVariables();
        }
    }
}

void MainPresenter::onSelectedConstantChanged(int index) {
    if (index == -1)
        currentConstant.clear();
    else
        currentConstant = constantMapping.at(index);
}

void MainPresenter::onConstantChanged(const std::string &name, const std::string &value) {
    ArithmeticType convertedValue;
    try {
        convertedValue = fromDecimal(value);
    }
    catch (const std::exception &e) {
        std::string error = "Failed to parse ";
        error += value;
        error += " as decimal.";
        view.showWarningDialog("Error", error);

        if (currentConstant.empty())
            return;
        else
            convertedValue = symbolTable.getConstants().at(currentConstant);
    }

    if (currentConstant.empty()) {
        //Create
        if (name.empty()) {
            view.showWarningDialog("Error", "The constant name cannot be empty.");
        } else {
            if (symbolTable.hasVariable(name)) {
                view.showWarningDialog("Error",
                                       "Variable with the name " + name + " already exists.");
            } else if (symbolTable.hasConstant(name)) {
                view.showWarningDialog("Error",
                                       "Constant with the name " + name + " already exists.");
            } else if (symbolTable.hasFunction(name)) {
                view.showWarningDialog("Error",
                                       "Function with the name " + name + " already exists.");
            } else if (symbolTable.hasScript(name)) {
                view.showWarningDialog("Error",
                                       "Script with the name " + name + " already exists.");
            } else {
                symbolTable.setConstant(name, convertedValue);
                applyConstants();
            }
        }
    } else {
        if (name.empty()) {
            //Delete
            if (view.showQuestionDialog("Delete Constant",
                                        "Do you want to delete " + currentConstant + " ?")) {
                symbolTable.remove(currentConstant);
                currentConstant.clear();
                applyConstants();
            }
        } else {
            //Update
            if (currentConstant != name) {
                //Update name
                if (symbolTable.hasVariable(name)) {
                    view.showWarningDialog("Error",
                                           "Variable with the name " + name + " already exists.");
                } else if (symbolTable.hasConstant(name)) {
                    view.showWarningDialog("Error",
                                           "Constant with the name " + name + " already exists.");
                } else if (symbolTable.hasFunction(name)) {
                    view.showWarningDialog("Error",
                                           "Function with the name " + name + " already exists.");
                } else if (symbolTable.hasScript(name)) {
                    view.showWarningDialog("Error",
                                           "Script with the name " + name + " already exists.");
                } else {
                    symbolTable.remove(currentConstant);
                    symbolTable.setConstant(name, convertedValue);
                    currentConstant = name;
                }
            } else {
                //Update value
                symbolTable.setConstant(currentConstant, convertedValue);
            }
            applyConstants();
        }
    }
}

void MainPresenter::onSelectedFunctionChanged(int index) {
    if (index == -1)
        currentFunction.clear();
    else
        currentFunction = functionMapping.at(index);

    applyCurrentFunction();
}

void MainPresenter::onFunctionNameChanged(const std::string &value) {
    if (currentFunction.empty()) {
        //Create
        if (value.empty()) {
            view.showWarningDialog("Error", "Function name cannot be empty.");
        } else {
            if (symbolTable.hasVariable(value)) {
                view.showWarningDialog("Error",
                                       "Variable with the name " + value + " already exists.");
            } else if (symbolTable.hasConstant(value)) {
                view.showWarningDialog("Error",
                                       "Constant with the name " + value + " already exists.");
            } else if (symbolTable.hasFunction(value)) {
                view.showWarningDialog("Error",
                                       "Function with the name " + value + " already exists.");
            } else if (symbolTable.hasScript(value)) {
                view.showWarningDialog("Error",
                                       "Script with the name " + value + " already exists.");
            } else {
                symbolTable.setFunction(value, {});
                applyFunctions();
            }
        }
    } else {
        if (value.empty()) {
            //Delete
            if (view.showQuestionDialog("Delete Function",
                                        "Do you want to delete " + currentFunction + " ?")) {
                symbolTable.remove(currentFunction);
                currentFunction.clear();
                applyFunctions();
            }
        } else {
            //Update
            if (currentFunction != value) {
                //Update name
                if (symbolTable.hasVariable(value)) {
                    view.showWarningDialog("Error",
                                           "Variable with the name " + value + " already exists.");
                } else if (symbolTable.hasConstant(value)) {
                    view.showWarningDialog("Error",
                                           "Constant with the name " + value + " already exists.");
                } else if (symbolTable.hasFunction(value)) {
                    view.showWarningDialog("Error",
                                           "Function with the name " + value + " already exists.");
                } else if (symbolTable.hasScript(value)) {
                    view.showWarningDialog("Error",
                                           "Script with the name " + value + " already exists.");
                } else {
                    Function f = symbolTable.getFunctions().at(currentFunction);
                    symbolTable.remove(currentFunction);
                    symbolTable.setFunction(value, f);
                    currentFunction = value;
                }
            }
            applyFunctions();
        }
    }
}

void MainPresenter::onFunctionBodyChanged(const std::string &value) {
    assert(!currentFunction.empty());
    Function f = symbolTable.getFunctions().at(currentFunction);
    f.expression = value;
    symbolTable.setFunction(currentFunction, f);
}

void MainPresenter::onFunctionArgsChanged(const std::vector<std::string> &arguments) {
    assert(!currentFunction.empty());
    Function f = symbolTable.getFunctions().at(currentFunction);
    f.argumentNames = arguments;
    symbolTable.setFunction(currentFunction, f);
    view.setFunctionArgs(arguments);
}

void MainPresenter::onActionExit() {
    view.quit();
}

void MainPresenter::onActionAbout() {
    view.showAboutDialog();
}

void MainPresenter::onActionSettings() {
    SettingsDialogState state;

    state.settings = settings;

    state.addonMetadata = AddonHelper::getAvailableAddons(getAddonModulesDirectory().toStdString());

    if (view.showSettingsDialog(state, state)) {
        addonManager.setActiveAddons(state.settings.enabledAddonModules);
        settings = state.settings;
        applySettings();
    }
}

void MainPresenter::onActionShowKeyPad(bool show) {
    settings.showKeypad = show;
    view.setKeyPadVisibility(show);
}

void MainPresenter::onActionShowBitView(bool show) {
    settings.showBitView = show;
    view.setBitViewVisibility(show);
}

void MainPresenter::onActionImportSymbolTable() {
    std::string filepath;
    if (view.showFileChooserDialog("Import symbol table", true, filepath)) {
        try {
            addonManager.setActiveAddons({});

            symbolTable = Serializer::deserializeTable(IO::fileReadAllText(filepath));

            currentVariable.clear();
            currentConstant.clear();
            currentFunction.clear();

            addonManager.setActiveAddons(settings.enabledAddonModules);

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

void MainPresenter::onActionExportSymbolTable() {
    std::string filepath;
    if (view.showFileChooserDialog("Export symbol table", false, filepath)) {
        try {
            IO::fileWriteAllText(filepath, Serializer::serializeTable(symbolTable));
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

void MainPresenter::onActiveTabChanged(int tab) {
    settings.activeTab = tab;
}

void MainPresenter::applyCurrentValue() {
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

const SymbolTable &MainPresenter::getSymbolTable() {
    return symbolTable;
}

void MainPresenter::setSymbolTable(const SymbolTable &table) {
    this->symbolTable = table;
    currentVariable.clear();
    currentConstant.clear();
    currentFunction.clear();
    applySymbolTable();
}

void MainPresenter::onAddonLoadFail(const std::string &moduleName, const std::string &error) {
    view.showWarningDialog("Error", "Failed to load addon (" + moduleName + ") Error: " + error);
}

void MainPresenter::onAddonUnloadFail(const std::string &moduleName, const std::string &error) {
    view.showWarningDialog("Error", "Failed to unload addon (" + moduleName + ") Error: " + error);
}

void MainPresenter::autoUpdateValueFromInputText() {
    // Attempt to evaluate the current input text without symbol table and if successful set current value without
    // receiving input submit. When input submit is received the expression is evaluated with the symbol table
    // and added to the history.
    try {
        double v = ExpressionParser::evaluate(inputText);
        currentValue = v;
        applyCurrentValue();
    }
    catch (const std::runtime_error &e) {}
}

void MainPresenter::applySettings() {
    view.disconnectPresenter(*this);

    view.setKeyPadVisibility(settings.showKeypad);
    view.setBitViewVisibility(settings.showBitView);
    view.setActiveTab(settings.activeTab);
    view.setWindowSize(settings.windowSize);

    view.connectPresenter(*this);
}

void MainPresenter::applySymbolTable() {
    applyVariables();
    applyConstants();
    applyFunctions();
    applyScripts();
}

void MainPresenter::applyVariables() {
    view.disconnectPresenter(*this);

    variableMapping.clear();

    //Update mapping with indices into the vector passed to the view
    std::vector<std::pair<std::string, std::string>> tmp;
    int currentIndex = -1;

    int i = 0;
    for (auto &v : symbolTable.getVariables()) {
        variableMapping[i] = v.first;

        if (currentVariable == v.first)
            currentIndex = i;

        tmp.emplace_back(std::pair<std::string, std::string>(v.first, toDecimal(v.second)));
        i++;
    }

    view.setVariableListView(tmp);
    view.setSelectedVariable(currentIndex);

    view.connectPresenter(*this);
}

void MainPresenter::applyConstants() {
    view.disconnectPresenter(*this);

    constantMapping.clear();

    //Update mapping with indices into the vector passed to the view
    std::vector<std::pair<std::string, std::string>> tmp;
    int currentIndex = -1;

    int i = 0;
    for (auto &v : symbolTable.getConstants()) {
        constantMapping[i] = v.first;

        if (currentConstant == v.first)
            currentIndex = i;

        tmp.emplace_back(std::pair<std::string, std::string>(v.first, toDecimal(v.second)));
        i++;
    }

    view.setConstantsListView(tmp);
    view.setSelectedConstant(currentIndex);

    view.connectPresenter(*this);
}

void MainPresenter::applyFunctions() {
    view.disconnectPresenter(*this);

    functionMapping.clear();

    //Update mapping with indices into the vector passed to the view
    std::vector<std::string> tmp;
    int currentIndex = -1;

    int i = 0;
    for (auto &v : symbolTable.getFunctions()) {
        functionMapping[i] = v.first;

        if (currentFunction == v.first)
            currentIndex = i;

        tmp.emplace_back(v.first);
        i++;
    }

    view.setFunctionsListView(tmp);
    view.setSelectedFunction(currentIndex);

    if (currentFunction.empty()) {
        view.setFunctionBodyEnabled(false);
        view.setFunctionArgsSpinBoxEnabled(false);
        view.setFunctionBody("");
        view.setFunctionArgs({});
    } else {
        Function func = symbolTable.getFunctions().at(currentFunction);

        view.setFunctionBodyEnabled(true);
        view.setFunctionArgsSpinBoxEnabled(true);
        view.setFunctionBody(func.expression);
        view.setFunctionArgs(func.argumentNames);
    }

    view.connectPresenter(*this);
}

void MainPresenter::applyCurrentFunction() {
    view.disconnectPresenter(*this);

    int currentIndex = -1;
    for (auto &v : functionMapping) {
        if (v.second == currentFunction) {
            currentIndex = v.first;
            break;
        }
    }

    view.setSelectedFunction(currentIndex);

    if (currentFunction.empty()) {
        view.setFunctionBodyEnabled(false);
        view.setFunctionArgsSpinBoxEnabled(false);
        view.setFunctionBody("");
        view.setFunctionArgs({});
    } else {
        Function func = symbolTable.getFunctions().at(currentFunction);

        view.setFunctionBodyEnabled(true);
        view.setFunctionArgsSpinBoxEnabled(true);
        view.setFunctionBody(func.expression);
        view.setFunctionArgs(func.argumentNames);
    }

    view.connectPresenter(*this);
}

void MainPresenter::applyScripts() {
    view.disconnectPresenter(*this);

    view.connectPresenter(*this);
}

void MainPresenter::applyCurrentScript() {
    view.disconnectPresenter(*this);

    view.connectPresenter(*this);
}