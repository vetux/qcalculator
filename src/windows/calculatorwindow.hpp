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

#ifndef QCALC_MAINWINDOW_HPP
#define QCALC_MAINWINDOW_HPP

#include <QMainWindow>
#include <QPushButton>
#include <QTableWidget>
#include <QSpinBox>
#include <QComboBox>

#include <bitset>
#include <set>

#include "addon/addonmanager.hpp"
#include "settings/settings.hpp"

#include "math/symboltable.hpp"

#include "widgets/symbolseditor.hpp"
#include "widgets/historywidget.hpp"

#include "windows/symbolseditorwindow.hpp"
#include "windows/pythonconsolewindow.hpp"
#include "windows/settingsdialog.hpp"
#include "windows/calculatorwindowactions.hpp"

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

    void onInputReturnPressed();

    void onInputTextChanged();

    void onInputTextEdited();

    void onSymbolTableChanged(const SymbolTable &symbolTable);

    void onActionSettings();

    void onActionExit();

    void onActionAbout();

    void onActionAboutQt();

    void onActionAboutPython();

    void onActionClearSymbolTable();

    void onActionOpenSymbolTable();

    void onActionSaveSymbolTable();

    void onActionSaveAsSymbolTable();

    void onActionEditSymbolTable();

    void onActionSymbolTableHistory();

    void onActionOpenTerminal();

    void onActionCompressDirectory();

    void onActionExtractArchive();

    void onActionCreateAddonBundle();

    void onHistoryTextDoubleClicked(const QString &text);

    void onSettingsAccepted();

    void onSettingsCancelled();

    void onActionClearHistory();

    void insertInputText(const QString &text);

private:
    void keyPressEvent(QKeyEvent *event) override;

private slots:

    void onInputCursorPositionChanged(int oldPos, int newPos);

    void onEvaluatePython(const std::string &expr, Interpreter::ParseStyle style);

private:

    QString evaluateExpression(const QString &expression);

    void loadSettings();

    void saveSettings();

    void applySettings();

    std::set<std::string> loadEnabledAddons(const QString &path);

    void saveEnabledAddons(const std::set<std::string> &addons);

    void loadSymbolTablePathHistory();

    void saveSymbolTablePathHistory();

    void setupMenuBar();

    void setupLayout();

    void setupDialogs();

    void cleanupDialogs();

    void updateSymbolHistoryMenu();

    bool loadSymbolTable(const std::string &path);

    bool saveSymbolTable(const std::string &path);

    void saveHistory();

    void loadHistory();

    void clearResultFromInputText();

    void runOnMainThread(const std::function<void()> &func, Qt::ConnectionType type);

private:
    QWidget *rootWidget{};
    HistoryWidget *historyWidget{};
    QLineEdit *input{};
    QLabel *inputMessage{};

    PythonConsoleWindow *terminalDialog = nullptr;
    SymbolsEditorWindow *symbolsDialog = nullptr;
    SettingsDialog *settingsDialog = nullptr;

    AddonManager addonManager;

    SymbolTable symbolTable;

    Settings settings;

    std::string previousResult;

    std::set<std::string> symbolTablePathHistory;
    std::string currentSymbolTablePath; // If the currently active symboltable was loaded from a file or saved to a file this path contains the path of the symbol table file.

    std::vector<std::pair<std::string, std::string>> history;

    bool inputTextContainsExpressionResult = false;
    int inputTextHistoryIndex = 0;
    std::string inputTextAppendedHistoryValue = "";

    CalculatorWindowActions actions;

    QCompleter *completer = nullptr;
    QStringListModel *completerModel = nullptr;
    QString completerWord;
};

#endif // QCALC_MAINWINDOW_HPP
