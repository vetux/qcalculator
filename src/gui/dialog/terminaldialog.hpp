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

#include <QDialog>

#include "gui/widgets/terminalwidget.hpp"

#include "cpython/interactiveinterpreter.hpp"

class TerminalDialog : public QDialog {
Q_OBJECT
public:
    explicit TerminalDialog(QWidget *parent = nullptr) : QDialog(parent) {
        term = new TerminalWidget();
        connect(term, SIGNAL(onReturnPressed()), this, SLOT(onTerminalReturnPressed()));
        setLayout(new QVBoxLayout());
        layout()->addWidget(term);
    }

private slots:

    void onTerminalReturnPressed() {
        std::string expr = term->getInputText().toStdString();
        try {
            auto res = InteractiveInterpreter::runString(expr);
            term->appendHistory(expr.c_str(), res.c_str());
        } catch (const std::exception &e) {
            term->appendHistory(expr.c_str(), e.what());
        }
    }

private:

    TerminalWidget *term;
};

#endif //QCALC_TERMINALDIALOG_HPP
