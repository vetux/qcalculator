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

#include "symbolseditorwindow.hpp"

#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>

#include "gui/calculatorwindow.hpp"

SymbolsEditorWindow::SymbolsEditorWindow(const SymbolTable &symbols,
                                         CalculatorWindow *calcWindow)
        : QMainWindow(calcWindow) {
    setWindowTitle("Symbols Editor");

    auto *widget = new QWidget;
    widget->setLayout(new QVBoxLayout());

    editor = new SymbolsEditor(widget);

    editor->setSymbols(symbols);

    widget->layout()->addWidget(editor);

    widget->layout()->setMargin(0);

    setCentralWidget(widget);

    connect(editor,
            SIGNAL(onSymbolsChanged(const SymbolTable &)),
            this,
            SIGNAL(symbolsChanged(const SymbolTable &)));

    auto *exitAction = new QAction("Close Editor");
    auto *fileMenu = new QMenu("File");

    fileMenu->addAction(calcWindow->actionOpenSymbols);
    fileMenu->addMenu(calcWindow->menuOpenRecent);
    fileMenu->addAction(calcWindow->actionSaveSymbols);
    fileMenu->addAction(calcWindow->actionSaveAsSymbols);
    fileMenu->addSeparator();
    fileMenu->addAction(calcWindow->actionClearSymbols);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(calcWindow->menuHelp);

    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
}

void SymbolsEditorWindow::setSymbols(const SymbolTable &symbols) {
    editor->setSymbols(symbols);
}

void SymbolsEditorWindow::setCurrentSymbolsPath(const std::string &path) {
    setWindowTitle(("Symbols Editor - " + path).c_str());
}
