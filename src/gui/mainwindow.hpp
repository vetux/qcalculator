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

#ifndef QCALC_MAINWINDOW_HPP
#define QCALC_MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>

#include <bitset>
#include <set>

#include "../addon/addonmanagerlistener.hpp"
#include "../io/settings.hpp"

#include "../math/symboltable.hpp"
#include "../math/numeralsystem.hpp"

#include "widgets/symbolseditor.hpp"
#include "widgets/historywidget.hpp"

#include "dialog/symbolsdialog.hpp"

class MainWindow : public QMainWindow, public AddonManagerListener {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void onAddonLoadFail(const std::string &moduleName, const std::string &error) override;

    void onAddonUnloadFail(const std::string &moduleName, const std::string &error) override;

signals:

    /**
     * Emitted whenever the input text for the main window changed.
     * This is emitted after evaluating an expression and is per default connected to the line edit set text slot.
     *
     * @param text
     */
    void signalInputTextChange(const QString &text);

    /**
     * Emitted whenever a expression was evaluated by the main window.
     * This is emitted after evaluating an expression and is per default connected to the history widget addContent slot.
     *
     * pair.first is the expression and pair.second is the result in decimal string form.
     *
     * @param expression
     * @param value
     */
    void signalExpressionEvaluated(const QString &expression, const QString &value);

public slots:

    /**
     * This slot evaluates the input text when invoked.
     */
    void onInputReturnPressed();

    void onSymbolTableChanged(const SymbolTable &symbolTable);

    void onActionSettings();

    void onActionExit();

    void onActionAbout();

    void onActionImportSymbolTable();

    void onActionExportSymbolTable();

    void onActionEditSymbolTable();

    void onActionSymbolTableHistory();

    const SymbolTable &getSymbolTable();

    void onHistoryTextDoubleClicked(const QString &text);

private:
    void loadSettings();

    void saveSettings();

    void loadSymbolTablePathHistory();

    void saveSymbolTablePathHistory();

    void setupMenuBar();

    void setupLayout();

    void updateSymbolHistoryMenu();

    bool importSymbolTable(const std::string &path);

    QWidget *rootWidget{};
    HistoryWidget *history{};
    QLineEdit *input{};

    QMenu *menuFile{};
    QMenu *menuSymbols{};
    QMenu *menuHelp{};

    QMenu *menuSymbolsHistory{};

    QAction *actionSettings{};
    QAction *actionExit{};

    QAction *actionEditSymbols{};
    QAction *actionImportSymbols{};
    QAction *actionExportSymbols{};

    QAction *actionAbout{};

    SymbolsDialog *symbolsDialog = nullptr;

    SymbolTable symbolTable;

    Settings settings;

    std::set<std::string> symbolTablePathHistory;
};

#endif // QCALC_MAINWINDOW_HPP
