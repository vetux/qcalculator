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

#include "gui/dialog/symbolsdialog.hpp"

#include <QVBoxLayout>

SymbolsDialog::SymbolsDialog(const SymbolTable &symbols,
                             int formattingPrecision,
                             int precision,
                             QWidget *parent)
        : QDialog(parent) {
    setModal(false);
    setWindowTitle("Symbols");
    setLayout(new QVBoxLayout());

    editor = new SymbolsEditor(this);

    editor->setSymbols(symbols);
    editor->setFormattingPrecision(formattingPrecision);
    editor->setPrecision(precision);

    layout()->addWidget(editor);

    connect(editor,
            SIGNAL(onSymbolsChanged(const SymbolTable &)),
            this,
            SIGNAL(symbolsChanged(const SymbolTable &)));
}

void SymbolsDialog::setSymbols(const SymbolTable &symbols) {
    editor->setSymbols(symbols);
}

void SymbolsDialog::setSymbolsFormattingPrecision(int precision) {
    editor->setFormattingPrecision(precision);
}

void SymbolsDialog::setSymbolsPrecision(int precision) {
    editor->setPrecision(precision);
}
