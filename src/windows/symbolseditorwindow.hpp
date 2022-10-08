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

#ifndef QCALC_SYMBOLSDIALOG_HPP
#define QCALC_SYMBOLSDIALOG_HPP

#include <QMainWindow>

#include "widgets/symbolseditor.hpp"

class CalculatorWindow;

class SymbolsEditorWindow : public QMainWindow {
Q_OBJECT
signals:

    void symbolsChanged(const SymbolTable &symbols);

public slots:

    void setSymbols(const SymbolTable &symbols);

    void setCurrentSymbolsPath(const std::string &path);

public:
    SymbolsEditorWindow(const SymbolTable &symbols, CalculatorWindow *calcWindow);

private:
    SymbolsEditor *editor;
};

#endif //QCALC_SYMBOLSDIALOG_HPP
