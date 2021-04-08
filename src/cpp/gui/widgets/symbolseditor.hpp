#ifndef QCALC_SYMBOLSEDITOR_HPP
#define QCALC_SYMBOLSEDITOR_HPP

#include <QWidget>

#include "calc/symboltable.hpp"

#include "gui/widgets/namedvalueeditor.hpp"
#include "gui/widgets/functionseditor.hpp"
#include "gui/widgets/scriptseditor.hpp"

class SymbolsEditor : public QWidget {
Q_OBJECT
public:
    explicit SymbolsEditor(QWidget *parent = nullptr);

public slots:

    void setSymbols(const SymbolTable &symbolTable);

signals:

    void onSymbolsChanged(const SymbolTable &symbolTable);

private slots:
    void onVariableAdded(const QString &name, const QString &value);

    void onVariableNameChanged(const QString &originalName, const QString &name);

    void onVariableValueChanged(const QString &name, const QString &value);

    void onConstantAdded(const QString &name, const QString &value);

    void onConstantNameChanged(const QString &originalName, const QString &name);

    void onConstantValueChanged(const QString &name, const QString &value);

private:
    SymbolTable symbolTable;
    NamedValueEditor *variablesEditor;
    NamedValueEditor *constantsEditor;
    FunctionsEditor *functionsEditor;
    ScriptsEditor *scriptsEditor;
};

#endif //QCALC_SYMBOLSEDITOR_HPP
