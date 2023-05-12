/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef QCALC_FUNCTIONSEDITOR_HPP
#define QCALC_FUNCTIONSEDITOR_HPP

#include <QWidget>
#include <QTableWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>

#include "calculator/function.hpp"

class FunctionsEditor : public QWidget {
Q_OBJECT
public:
    explicit FunctionsEditor(QWidget *parent = nullptr);

public slots:

    void setFunctions(const std::map<std::string, Function> &functions);

    void setCurrentFunction(const QString &name);

signals:

    void onFunctionAdded(const QString &name);

    void onFunctionNameChanged(const QString &originalName, const QString &name);

    void onFunctionBodyChanged(const QString &name, const QString &body);

    void onFunctionArgsChanged(const QString &name, const std::vector<std::string> &args);

    void onCurrentFunctionChanged(const QString &name);

private slots:

    void onFunctionAddPressed();

    void onFunctionArgEditingFinished();

    void onFunctionArgsSpinBoxChanged(int value);

    void onTableCellActivated(int row, int column);

    void onTableCellChanged(int row ,int column);

    void onFunctionExpressionChanged();

private:
    void applyArgs(const std::vector<std::string> &args);

    std::map<std::string, Function> functions;

    std::map<std::string, int> rowMapping;

    std::string currentFunction;

    QTableWidget *list;

    QLineEdit *addLineEdit;
    QPushButton *addPushButton;

    QSpinBox *argsSpinBox;
    QLineEdit *argEdit0;
    QLineEdit *argEdit1;
    QLineEdit *argEdit2;
    QLineEdit *argEdit3;
    QLineEdit *argEdit4;

    QTextEdit *expressionEdit;
};

#endif //QCALC_FUNCTIONSEDITOR_HPP
