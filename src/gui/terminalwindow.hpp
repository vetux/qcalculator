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

#ifndef QCALC_TERMINALDIALOG_HPP
#define QCALC_TERMINALDIALOG_HPP

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>

#include "gui/widgets/terminalwidget.hpp"

#include "pycx/interpreter.hpp"

class TerminalWindow : public QMainWindow {
Q_OBJECT
public:
    explicit TerminalWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        term = new TerminalWidget();
        connect(term, SIGNAL(onReturnPressed()), this, SLOT(onTerminalReturnPressed()));
        auto *widget = new QWidget;
        widget->setLayout(new QVBoxLayout());
        widget->layout()->addWidget(term);
        widget->layout()->setMargin(0);
        setCentralWidget(widget);

        auto *menu = new QMenu("File");
        auto *action = new QAction("Exit");
        menu->addAction(action);
        menuBar()->addMenu(menu);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(close()));
    }

private slots:

    void onTerminalReturnPressed() {
        std::string expr = term->getInputText().toStdString();
        try {
            auto res = Interpreter::runString(expr, term->getMultiLineInput() ? Interpreter::FILE_INPUT : Interpreter::SINGLE_INPUT);
            term->appendHistory(expr.c_str(), res.c_str());
        } catch (const std::exception &e) {
            term->appendHistory(expr.c_str(), e.what());
        }
    }

private:
    TerminalWidget *term;
};

#endif //QCALC_TERMINALDIALOG_HPP
