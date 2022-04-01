/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_SYMBOLSEDITOR_HPP
#define QCALC_SYMBOLSEDITOR_HPP

#include <QWidget>

#include "../../math/symboltable.hpp"

#include "namedvalueeditor.hpp"
#include "functionseditor.hpp"
#include "scriptseditor.hpp"

/**
 * The symbols editor exposes and parses the variable and constant values with
 * a fixed precision of 100 digits (The maximum user configurable precision),
 * and always with nearest rounding.
 */
class SymbolsEditor : public QWidget {
Q_OBJECT
public:
    explicit SymbolsEditor(QWidget *parent = nullptr);

public slots:

    void setSymbols(const SymbolTable &symbolTable);

    void setFormattingPrecision(int precision);

    void setPrecision(int precision);

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

    NamedValueEditor *variablesEditor;
    NamedValueEditor *constantsEditor;
    FunctionsEditor *functionsEditor;
    ScriptsEditor *scriptsEditor;

    QString currentFunction;

    int precision = MPFR_PREC_MIN;
    int formattingPrecision = 0;
};

#endif //QCALC_SYMBOLSEDITOR_HPP
