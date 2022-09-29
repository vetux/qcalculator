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
#include <QSpinBox>
#include <QComboBox>

#include <bitset>
#include <set>
#include <bits/stl_set.h>

#include "addon/addonmanager.hpp"
#include "settings/settings.hpp"

#include "math/symboltable.hpp"
#include "math/numeralsystem.hpp"

#include "widgets/symbolseditor.hpp"
#include "widgets/historywidget.hpp"

#include "symbolseditorwindow.hpp"
#include "terminalwindow.hpp"

class CalculatorWindow : public QMainWindow {
Q_OBJECT

public:
    explicit CalculatorWindow(QWidget *parent = nullptr);

    ~CalculatorWindow() override;

    void closeEvent(QCloseEvent *event) override;

    void resizeEvent(QResizeEvent *event) override;

    void onAddonLoadFail(const std::string &moduleName, const std::string &error);

    void onAddonUnloadFail(const std::string &moduleName, const std::string &error);

    const SymbolTable &getSymbolTable();

signals:
    void signalExpressionEvaluated(const QString &expression, const QString &value);

public slots:

    void onEvaluateExpression(const QString &expression);

    void onInputReturnPressed();

    void onSymbolTableChanged(const SymbolTable &symbolTable);

    void onActionSettings();

    void onActionExit();

    void onActionAbout();

    void onActionAboutQt();

    void onActionOpenSymbolTable();

    void onActionSaveSymbolTable();

    void onActionSaveAsSymbolTable();

    void onActionEditSymbolTable();

    void onActionSymbolTableHistory();

    void onActionOpenTerminal();

    void onHistoryTextDoubleClicked(const QString &text);

private:
    QString evaluateExpression(const QString &expression);

    void loadSettings();

    void saveSettings();

    std::set<std::string> loadEnabledAddons();

    void saveEnabledAddons(const std::set<std::string> &addons);

    void loadSymbolTablePathHistory();

    void saveSymbolTablePathHistory();

    void setupMenuBar();

    void setupLayout();

    void updateSymbolHistoryMenu();

    bool importSymbolTable(const std::string &path);

    bool saveSymbolTable(const std::string &path);

    QWidget *rootWidget{};
    HistoryWidget *history{};
    QLineEdit *input{};

    QMenu *menuFile{};
    QMenu *menuSymbols{};
    QMenu *menuTools{};
    QMenu *menuHelp{};

    QMenu *menuOpenRecent{};

    QAction *actionSettings{};
    QAction *actionExit{};

    QAction *actionOpenTerminal{};

    QAction *actionEditSymbols{};
    QAction *actionOpenSymbols{};
    QAction *actionSaveSymbols{};
    QAction *actionSaveAsSymbols{};

    QAction *actionAbout{};
    QAction *actionAboutQt{};

    SymbolsEditorWindow *symbolsDialog = nullptr;

    SymbolTable symbolTable;

    Settings settings;

    std::set<std::string> symbolTablePathHistory;
    std::string currentSymbolTablePath; // If the currently active symboltable was loaded from a file or saved to a file this path contains the path of the symbol table file.

    std::unique_ptr<AddonManager> addonManager;

    QString enabledAddonsFilePath;
};

#endif // QCALC_MAINWINDOW_HPP
