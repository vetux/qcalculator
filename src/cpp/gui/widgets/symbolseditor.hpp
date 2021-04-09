#ifndef QCALC_SYMBOLSEDITOR_HPP
#define QCALC_SYMBOLSEDITOR_HPP

#include <QWidget>

#include "math/symboltable.hpp"

#include "gui/widgets/namedvalueeditor.hpp"
#include "gui/widgets/functionseditor.hpp"
#include "gui/widgets/scriptseditor.hpp"

class SymbolsEditor : public QWidget {
Q_OBJECT
public:
    explicit SymbolsEditor(QWidget *parent = nullptr);

public slots:

    void setSymbols(const SymbolTable &symbolTable);

    /**
     * Set the precision to use when converting user input to ArithmeticType instances.
     *
     * @param precision
     */
    void setConversionPrecision(int precision);

signals:

    void onSymbolsChanged(const SymbolTable &symbolTable);

private slots:

    void onVariableAdded(const QString &name, const QString &value);

    void onVariableNameChanged(const QString &originalName, const QString &name);

    void onVariableValueChanged(const QString &name, const QString &value);

    void onConstantAdded(const QString &name, const QString &value);

    void onConstantNameChanged(const QString &originalName, const QString &name);

    void onConstantValueChanged(const QString &name, const QString &value);

    void onFunctionAdded(const QString &name);

    void onFunctionNameChanged(const QString &originalName, const QString &name);

    void onFunctionBodyChanged(const QString &name, const QString &body);

    void onFunctionArgsChanged(const QString &name, const std::vector<std::string> &args);

    void onCurrentFunctionChanged(const QString &name);

private:
    SymbolTable symbolTable;

    int convertPrecision;

    NamedValueEditor *variablesEditor;
    NamedValueEditor *constantsEditor;
    FunctionsEditor *functionsEditor;
    ScriptsEditor *scriptsEditor;

    QString currentFunction;
};

#endif //QCALC_SYMBOLSEDITOR_HPP
