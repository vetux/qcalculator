#include "gui/widgets/symbolseditor.hpp"

#include <QTabWidget>
#include <QVBoxLayout>
#include <QMessageBox>

#include "numberformat.hpp"

std::map<QString, QString> convertMap(std::map<std::string, ArithmeticType> map) {
    std::map<QString, QString> ret;
    for (auto &p : map) {
        ret[QString(p.first.c_str())] = NumberFormat::toDecimal(p.second).c_str();
    }
    return ret;
}

SymbolsEditor::SymbolsEditor(QWidget *parent) : QWidget(parent) {
    setLayout(new QVBoxLayout(this));

    layout()->setContentsMargins(3, 3, 3, 3);

    auto *tabs = new QTabWidget(this);

    variablesEditor = new NamedValueEditor(tabs);
    constantsEditor = new NamedValueEditor(tabs);
    functionsEditor = new FunctionsEditor(tabs);
    scriptsEditor = new ScriptsEditor(tabs);

    tabs->addTab(variablesEditor, "Variables");
    tabs->addTab(constantsEditor, "Constants");
    tabs->addTab(functionsEditor, "Functions");
    tabs->addTab(scriptsEditor, "Scripts");

    layout()->addWidget(tabs);

    connect(variablesEditor,
            SIGNAL(onNamedValueAdded(const QString &, const QString &)),
            this,
            SLOT(onVariableAdded(const QString &, const QString &)));
    connect(variablesEditor,
            SIGNAL(onNameChanged(const QString &, const QString &)),
            this,
            SLOT(onVariableNameChanged(const QString &, const QString &)));
    connect(variablesEditor,
            SIGNAL(onValueChanged(const QString &, const QString &)),
            this,
            SLOT(onVariableValueChanged(const QString &, const QString &)));

    connect(constantsEditor,
            SIGNAL(onNamedValueAdded(const QString &, const QString &)),
            this,
            SLOT(onConstantAdded(const QString &, const QString &)));
    connect(constantsEditor,
            SIGNAL(onNameChanged(const QString &, const QString &)),
            this,
            SLOT(onConstantNameChanged(const QString &, const QString &)));
    connect(constantsEditor,
            SIGNAL(onValueChanged(const QString &, const QString &)),
            this,
            SLOT(onConstantValueChanged(const QString &, const QString &)));

    connect(functionsEditor,
            SIGNAL(onFunctionAdded(const QString &)),
            this,
            SLOT(onFunctionAdded(const QString &)));
    connect(functionsEditor,
            SIGNAL(onFunctionNameChanged(const QString &, const QString &)),
            this,
            SLOT(onFunctionNameChanged(const QString &, const QString &)));
    connect(functionsEditor,
            SIGNAL(onFunctionBodyChanged(const QString &, const QString &)),
            this,
            SLOT(onFunctionBodyChanged(const QString &, const QString &)));
    connect(functionsEditor,
            SIGNAL(onFunctionArgsChanged(const QString &, const std::vector<std::string> &)),
            this,
            SLOT(onFunctionArgsChanged(const QString &, const std::vector<std::string> &)));
    connect(functionsEditor,
            SIGNAL(onCurrentFunctionChanged(const QString &)),
            this,
            SLOT(onCurrentFunctionChanged(const QString &)));
}

void SymbolsEditor::setSymbols(const SymbolTable &symtable) {
    symbolTable = symtable;

    variablesEditor->setValues(convertMap(symtable.getVariables()));
    constantsEditor->setValues(convertMap(symtable.getConstants()));
    functionsEditor->setFunctions(symtable.getFunctions());
    functionsEditor->setCurrentFunction(currentFunction);
    scriptsEditor->setScripts(symtable.getScripts());
}

void SymbolsEditor::onVariableAdded(const QString &name, const QString &value) {
    if (symbolTable.hasVariable(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add variable", "A variable with the name already exists.");
    } else if (symbolTable.hasConstant(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add variable", "A constant with the name already exists.");
    } else if (symbolTable.hasFunction(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add variable", "A function with the name already exists.");
    } else if (symbolTable.hasScript(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add variable", "A script with the name already exists.");
    } else {
        ArithmeticType valueConverted;
        if (value.isEmpty()) {
            valueConverted = 0;
        } else {
            try {
                valueConverted = NumberFormat::fromDecimal(value.toStdString());
            }
            catch (const std::exception &e) {
                valueConverted = 0;
                QMessageBox::warning(this, "Failed to convert value", "Failed to parse value as decimal.");
            }
        }
        symbolTable.setVariable(name.toStdString(), valueConverted);
        emit onSymbolsChanged(symbolTable);
    }
}

void SymbolsEditor::onVariableNameChanged(const QString &originalName, const QString &name) {
    if (symbolTable.hasVariable(name.toStdString())) {
        QMessageBox::warning(this, "Failed to changed variable name", "A variable with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else if (symbolTable.hasConstant(name.toStdString())) {
        QMessageBox::warning(this, "Failed to changed variable name", "A constant with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else if (symbolTable.hasFunction(name.toStdString())) {
        QMessageBox::warning(this, "Failed to changed variable name", "A function with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else if (symbolTable.hasScript(name.toStdString())) {
        QMessageBox::warning(this, "Failed to changed variable name", "A script with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else {
        ArithmeticType value = symbolTable.getVariables().at(originalName.toStdString());
        symbolTable.remove(originalName.toStdString());
        symbolTable.setVariable(name.toStdString(), value);
        emit onSymbolsChanged(symbolTable);
    }
}

void SymbolsEditor::onVariableValueChanged(const QString &name, const QString &value) {
    ArithmeticType originalValue = symbolTable.getVariables().at(name.toStdString());
    ArithmeticType newValue;
    try {
        newValue = NumberFormat::fromDecimal(value.toStdString());
    }
    catch (const std::exception &e) {
        newValue = originalValue;
        QMessageBox::warning(this, "Failed to convert value", "Failed to parse value as decimal.");
    }
    symbolTable.setVariable(name.toStdString(), newValue);
    emit onSymbolsChanged(symbolTable);
}

void SymbolsEditor::onConstantAdded(const QString &name, const QString &value) {
    if (symbolTable.hasVariable(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add constant", "A variable with the name already exists.");
    } else if (symbolTable.hasConstant(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add constant", "A constant with the name already exists.");
    } else if (symbolTable.hasFunction(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add constant", "A function with the name already exists.");
    } else if (symbolTable.hasScript(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add constant", "A script with the name already exists.");
    } else {
        ArithmeticType valueConverted;
        if (value.isEmpty()) {
            valueConverted = 0;
        } else {
            try {
                valueConverted = NumberFormat::fromDecimal(value.toStdString());
            }
            catch (const std::exception &e) {
                valueConverted = 0;
                QMessageBox::warning(this, "Failed to convert value", "Failed to parse value as decimal.");
            }
        }
        symbolTable.setConstant(name.toStdString(), valueConverted);
        emit onSymbolsChanged(symbolTable);
    }
}

void SymbolsEditor::onConstantNameChanged(const QString &originalName, const QString &name) {
    if (symbolTable.hasVariable(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change constant name", "A variable with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else if (symbolTable.hasConstant(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change constant name", "A constant with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else if (symbolTable.hasFunction(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change constant name", "A function with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else if (symbolTable.hasScript(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change constant name", "A script with the name already exists.");
        variablesEditor->setValues(convertMap(symbolTable.getVariables()));
    } else {
        ArithmeticType value = symbolTable.getConstants().at(originalName.toStdString());
        symbolTable.remove(originalName.toStdString());
        symbolTable.setConstant(name.toStdString(), value);
        emit onSymbolsChanged(symbolTable);
    }
}

void SymbolsEditor::onConstantValueChanged(const QString &name, const QString &value) {
    ArithmeticType originalValue = symbolTable.getConstants().at(name.toStdString());
    ArithmeticType newValue;
    try {
        newValue = NumberFormat::fromDecimal(value.toStdString());
    }
    catch (const std::exception &e) {
        newValue = originalValue;
        QMessageBox::warning(this, "Failed to convert value", "Failed to parse value as decimal.");
    }
    symbolTable.setConstant(name.toStdString(), newValue);
    emit onSymbolsChanged(symbolTable);
}

void SymbolsEditor::onFunctionAdded(const QString &name) {
    if (symbolTable.hasVariable(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add function", "A variable with the name already exists.");
    } else if (symbolTable.hasConstant(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add function", "A constant with the name already exists.");
    } else if (symbolTable.hasFunction(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add function", "A function with the name already exists.");
    } else if (symbolTable.hasScript(name.toStdString())) {
        QMessageBox::warning(this, "Failed to add function", "A script with the name already exists.");
    } else {
        symbolTable.setFunction(name.toStdString(), {});
        emit onSymbolsChanged(symbolTable);
    }
}

void SymbolsEditor::onFunctionNameChanged(const QString &originalName, const QString &name) {
    if (symbolTable.hasVariable(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change function name", "A variable with the name already exists.");
        functionsEditor->setFunctions(symbolTable.getFunctions());
        functionsEditor->setCurrentFunction(currentFunction);
    } else if (symbolTable.hasConstant(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change function name", "A constant with the name already exists.");
        functionsEditor->setFunctions(symbolTable.getFunctions());
        functionsEditor->setCurrentFunction(currentFunction);
    } else if (symbolTable.hasFunction(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change function name", "A function with the name already exists.");
        functionsEditor->setFunctions(symbolTable.getFunctions());
        functionsEditor->setCurrentFunction(currentFunction);
    } else if (symbolTable.hasScript(name.toStdString())) {
        QMessageBox::warning(this, "Failed to change function name", "A script with the name already exists.");
        functionsEditor->setFunctions(symbolTable.getFunctions());
        functionsEditor->setCurrentFunction(currentFunction);
    } else {
        Function f = symbolTable.getFunctions().at(originalName.toStdString());
        symbolTable.remove(originalName.toStdString());
        symbolTable.setFunction(name.toStdString(), f);
        emit onSymbolsChanged(symbolTable);
    }
}

void SymbolsEditor::onFunctionBodyChanged(const QString &name, const QString &body) {
    Function f = symbolTable.getFunctions().at(name.toStdString());
    f.expression = body.toStdString();
    symbolTable.setFunction(name.toStdString(), f);
    emit onSymbolsChanged(symbolTable);
}

void SymbolsEditor::onFunctionArgsChanged(const QString &name, const std::vector<std::string> &args) {
    Function f = symbolTable.getFunctions().at(name.toStdString());
    f.argumentNames = args;
    symbolTable.setFunction(name.toStdString(), f);
    emit onSymbolsChanged(symbolTable);
}

void SymbolsEditor::onCurrentFunctionChanged(const QString &name) {
    currentFunction = name;
}
