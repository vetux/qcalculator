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

#ifndef QCALC_TERMINALDIALOG_HPP
#define QCALC_TERMINALDIALOG_HPP

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include "widgets/terminalwidget.hpp"
#include "windows/calculatorwindowactions.hpp"

#include "python/interpreter.hpp"

#include "io/paths.hpp"

class PythonConsoleWindow : public QMainWindow {
Q_OBJECT
public:
    explicit PythonConsoleWindow(CalculatorWindowActions &actions, QWidget *parent = nullptr) : QMainWindow(parent) {
        term = new TerminalWidget();
        connect(term, SIGNAL(onReturnPressed()), this, SLOT(onTerminalReturnPressed()));
        auto *widget = new QWidget;
        widget->setLayout(new QVBoxLayout());
        widget->layout()->addWidget(term);
        widget->layout()->setMargin(0);
        setCentralWidget(widget);

        auto *menu = new QMenu("File");
        auto *action = new QAction("Close Window");
        menu->addAction(action);
        menuBar()->addMenu(menu);
        menu = new QMenu("Tools");
        menu->addAction(actions.actionCompressDirectory);
        menu->addAction(actions.actionExtractArchive);
        menu->addSeparator();
        menu->addAction(actions.actionCreateAddonBundle);
        menuBar()->addMenu(menu);
        menuBar()->addMenu(actions.menuHelp);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(close()));

        setWindowTitle("Python Console");

        setWindowIcon(QIcon(Paths::getTerminalIconFile().c_str()));
    }

signals:

    void evaluatePython(const std::string &expr, Interpreter::ParseStyle parseStyle);

public slots:

    void printError(const QString &err) {
        term->printError(err);
    }

    void printOutput(const QString &out) {
        term->printOutput(out);
    }

private slots:

    void onTerminalReturnPressed() {
        term->printOutput(">>> " + term->getInputText() + "\n");
        emit evaluatePython(term->getInputText().toStdString(), term->getMultiLineInput() ? Interpreter::FILE_INPUT : Interpreter::SINGLE_INPUT);
        term->setInputText("");
    }

private:
    TerminalWidget *term;
};

#endif //QCALC_TERMINALDIALOG_HPP
