/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
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

#include "windows/symbolseditorwindow.hpp"

#include <QVBoxLayout>
#include <QMenu>
#include <QMenuBar>

#include "windows/calculatorwindowactions.hpp"

#include "io/paths.hpp"

#define WINDOW_TITLE "Symbols Editor"
static const auto *WINDOW_TITLE_PREFIX = WINDOW_TITLE " - ";
static const auto *WINDOW_TITLE_PREFIX_UNSAVED = WINDOW_TITLE " - *";

SymbolsEditorWindow::SymbolsEditorWindow(const SymbolTable &symbols,
                                         CalculatorWindowActions &actions,
                                         QWidget *parent)
        : QMainWindow(parent),
          actions(actions) {
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

    auto *exitAction = new QAction("Close Window");
    auto *fileMenu = new QMenu("File");

    fileMenu->addAction(actions.actionOpenSymbols);
    fileMenu->addMenu(actions.menuOpenRecent);
    fileMenu->addAction(actions.actionSaveSymbols);
    fileMenu->addAction(actions.actionSaveAsSymbols);
    fileMenu->addSeparator();
    fileMenu->addAction(actions.actionClearSymbols);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(actions.menuHelp);

    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));
    connect(actions.actionSaveSymbols, SIGNAL(triggered(bool)), this, SLOT(onSaveSymbolsTriggered()));

    setWindowIcon(QIcon(Paths::getSymbolsIconFile().c_str()));
}

void SymbolsEditorWindow::setSymbols(const SymbolTable &symbols, bool modified, const std::string &path) {
    symbolsPath = path;
    editor->setSymbols(symbols);
    if (!path.empty()) {
        if (modified) {
            setWindowTitle((WINDOW_TITLE_PREFIX_UNSAVED + path).c_str());
        } else {
            setWindowTitle((WINDOW_TITLE_PREFIX + path).c_str());
        }
    } else {
        setWindowTitle(WINDOW_TITLE);
    }
}

void SymbolsEditorWindow::onSaveSymbolsTriggered() {
    setWindowTitle((WINDOW_TITLE_PREFIX + symbolsPath).c_str());
}
