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

#include <QDialog>

#include "gui/widgets/symbolseditor.hpp"

class SymbolsDialog : public QDialog {
Q_OBJECT
signals:

    void symbolsChanged(const SymbolTable &symbols);

public slots:

    void setSymbols(const SymbolTable &symbols);

    void setSymbolsFormattingPrecision(int precision);

    void setSymbolsPrecision(int precision);

public:
    SymbolsDialog(const SymbolTable &symbols, int formattingPrecision, int precision, QWidget *parent);

private:
    SymbolsEditor* editor;
};

#endif //QCALC_SYMBOLSDIALOG_HPP
