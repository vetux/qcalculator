#include "model.hpp"

#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QFileInfo>

#include "numberformat.hpp"
#include "serializer.hpp"

#define SETTINGS_FILENAME "/settings.json"

Model::Model()
        : listener(nullptr) {
}

void Model::setStateListener(StateListener &l) {
    this->listener = &l;
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

void Model::importSymbolTable(const std::string &filePath) {
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

    state.symbolTable = Serializer::deserializeTable(contents.toStdString());

    state.currentVariable = -1;
    state.currentConstant = -1;
    state.currentFunction = -1;
    state.currentScript = -1;

    listener->onStateCurrentVariableChanged(state.currentVariable);
    listener->onStateCurrentConstantChanged(state.currentConstant);
    listener->onStateCurrentFunctionChanged(state.currentFunction);
    listener->onStateCurrentScriptChanged(state.currentScript);

    listener->onStateSymbolTableChanged(state.symbolTable);
}

void Model::exportSymbolTable(const std::string &filePath) {
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
    QString contents = Serializer::serializeTable(state.symbolTable).c_str();
    stream << contents;
    stream.flush();

    file.close();
}

void Model::loadSettings() {
    QString appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString fileName = appDir;
    fileName.append(SETTINGS_FILENAME);
    try {
        QFile file(fileName);
        if (file.exists()) {
            file.open(QFile::ReadOnly);

            QTextStream stream(&file);
            QString settingsContents = stream.readAll();
            stream.flush();

            file.close();

            state = Serializer::deserializeSettings(settingsContents.toStdString());
        }
    }
    catch (std::exception &e) {
        std::string error = "Failed to read settings file at ";
        error += fileName.toStdString();
        error += " Error: ";
        error += e.what();
        throw std::runtime_error(error);
    }

    listener->onStateValueChanged(state.value);
    listener->onStateHistoryChanged(state.history);
    listener->onStateInputChanged(state.input);
    listener->onStateSymbolTableChanged(state.symbolTable);
    listener->onStateShowKeyPadChanged(state.showKeypad);
    listener->onStateShowBitViewChanged(state.showBitView);
    listener->onStateDockPositionChanged(
            state.dockPosition); //Setting dock visibility before setting dock position leaves the dock invisible.
    listener->onStateShowDockChanged(state.showDock);
    listener->onStateHistoryLimitChanged(state.historyLimit);
    listener->onStateDockSelectedTabChanged(state.dockSelectedTab);
    listener->onStateWindowSizeChanged(state.windowSize);
    listener->onStateCurrentVariableChanged(state.currentVariable);
    listener->onStateCurrentConstantChanged(state.currentConstant);
    listener->onStateCurrentFunctionChanged(state.currentFunction);
    listener->onStateCurrentScriptChanged(state.currentScript);
}

void Model::saveSettings() {
    QString appDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString fileName = appDir;
    fileName.append(SETTINGS_FILENAME);
    try {
        if (!QDir(appDir).exists())
            QDir().mkpath(appDir);

        QFile file(fileName);

        file.open(QFile::WriteOnly | QFile::Truncate);

        QTextStream stream(&file);
        QString str = Serializer::serializeSettings(state).c_str();
        stream << str;
        stream.flush();

        file.close();
    }
    catch (std::exception &e) {
        std::string error = "Failed to write settings file at ";
        error += fileName.toStdString();
        error += " Error: ";
        error += e.what();
        throw std::runtime_error(error);
    }
}

const State &Model::getState() {
    return state;
}
