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

#include "mainwindow.hpp"

#include <filesystem>

#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QApplication>
#include <QProcess>

#include "addon/addonmanager.hpp"

#include "io/paths.hpp"
#include "io/serializer.hpp"
#include "io/fileoperations.hpp"
#include "settings/settingconstants.hpp"

#include "math/expressionparser.hpp"

#include "dialog/settings/settingsdialog.hpp"
#include "dialog/symbolsdialog.hpp"
#include "dialog/aboutdialog.hpp"

#include "widgets/historywidget.hpp"
#include "widgets/symbolseditor.hpp"

#include "pycx/modules/exprtkmodule.hpp"
#include "pycx/modules/stdredirmodule.hpp"

#include "pycx/interpreter.hpp"

#include "extern/exprtk_mpdecimal_adaptor.hpp"

static const std::string ADDONS_FILE = "/addons.json";
static const std::string SETTINGS_FILE = "/settings.json";
static const std::string SYMBOL_TABLE_HISTORY_FILE = "/symboltablehistory.json";

static const int MAX_FORMATTING_PRECISION = 100000;
static const int MAX_SYMBOL_TABLE_HISTORY = 100;

//TODO:Feature: Completion and history navigation for input line edit with eg. up / down arrows.
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setObjectName("MainWindow");

    setupLayout();
    setupMenuBar();

    input->setFocus();

    //Scale original point size by 1.3 and convert to integer (C++ Floating–integral conversion - fraction is truncated)
    QFont defaultFont = input->font();
    QFont largeFont(defaultFont.family(), (int) (defaultFont.pointSize() * 1.3));

    input->setFont(largeFont);
    history->setHistoryFont(largeFont);

    QPalette historyPalette = history->palette();
    historyPalette.setColor(history->backgroundRole(), input->palette().color(input->backgroundRole()));
    history->setPalette(historyPalette);

    connect(actionSettings, SIGNAL(triggered(bool)), this, SLOT(onActionSettings()));
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExit()));
    connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(onActionAbout()));
    connect(actionAboutQt, SIGNAL(triggered(bool)), this, SLOT(onActionAboutQt()));
    connect(actionOpenSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionOpenSymbolTable()));
    connect(actionSaveSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionSaveSymbolTable()));
    connect(actionSaveAsSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionSaveAsSymbolTable()));
    connect(actionEditSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionEditSymbolTable()));
    connect(actionOpenTerminal, SIGNAL(triggered(bool)), this, SLOT(onActionOpenTerminal()));

    connect(input, SIGNAL(returnPressed()), this, SLOT(onInputReturnPressed()));

    connect(history,
            SIGNAL(onTextDoubleClicked(const QString &)),
            this,
            SLOT(onHistoryTextDoubleClicked(const QString &)));

    loadSettings();

    loadSymbolTablePathHistory();
    saveSymbolTablePathHistory();

    updateSymbolHistoryMenu();

    ExprtkModule::setGlobalTable(symbolTable,
                                 [this]() {
                                     onSymbolTableChanged(symbolTable);
                                 });

    addonManager = std::make_unique<AddonManager>(Paths::getAddonDirectory(),
                                                  Paths::getLibDirectory(),
                                                  [this](const std::string &module, const std::string &error) {
                                                      return onAddonLoadFail(module, error);
                                                  },
                                                  [this](const std::string &module, const std::string &error) {
                                                      return onAddonUnloadFail(module, error);
                                                  });

    enabledAddonsFilePath = Paths::getAppDataDirectory().append(ADDONS_FILE).c_str();

    std::set<std::string> enabledAddons = loadEnabledAddons();

    //Check for enabled addons which dont exist anymore.
    std::set<std::string> availableAddons;
    auto addons = addonManager->getAvailableAddons();
    for (auto &addon: enabledAddons) {
        if (addons.find(addon) != addons.end())
            availableAddons.insert(addon);
    }

    addonManager->setActiveAddons(availableAddons);
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent *event) {
}

void MainWindow::resizeEvent(QResizeEvent *event) {
}

void MainWindow::onAddonLoadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to load module",
                         ("Module " + moduleName + " failed to load\n\n" + error).c_str());
}

void MainWindow::onAddonUnloadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to unload module",
                         ("Module " + moduleName + " failed to unload\n\n" + error).c_str());
}

void MainWindow::onEvaluateExpression(const QString &expression) {
    evaluateExpression(expression);
}

void MainWindow::onInputReturnPressed() {
    auto expr = input->text();
    auto res = evaluateExpression(expr);
    if (!res.isEmpty()) {
        input->setText(res);
        history->addContent(expr, res);
    }
}

void MainWindow::onSymbolTableChanged(const SymbolTable &symbolTableArg) {
    this->symbolTable = symbolTableArg;
    if (symbolsDialog != nullptr) {
        symbolsDialog->setSymbols(symbolTable);
    }
}

void MainWindow::onActionSettings() {
    SettingsDialog dialog(*addonManager);

    dialog.setEnabledAddons(addonManager->getActiveAddons());

    dialog.setPrecision(settings.value(SETTING_PRECISION).toInt());
    dialog.setExponentMin(settings.value(SETTING_EXPONENT_MIN).toInt());
    dialog.setExponentMax(settings.value(SETTING_EXPONENT_MAX).toInt());
    dialog.setRoundingMode(Serializer::deserializeRoundingMode(
            settings.value(SETTING_ROUNDING).toInt()));
    dialog.setShowInexactWarning(settings.value(SETTING_WARN_INEXACT).toInt());

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        settings.update(SETTING_PRECISION.key, dialog.getPrecision());
        settings.update(SETTING_EXPONENT_MAX.key, dialog.getExponentMax());
        settings.update(SETTING_EXPONENT_MIN.key, dialog.getExponentMin());
        settings.update(SETTING_ROUNDING.key, dialog.getRoundingMode());
        settings.update(SETTING_WARN_INEXACT.key, dialog.getShowInexactWarning());

        decimal::context.prec(settings.value(SETTING_PRECISION).toInt());
        decimal::context.round(settings.value(SETTING_ROUNDING).toInt());
        decimal::context.emax(settings.value(SETTING_EXPONENT_MAX).toInt());
        decimal::context.emin(settings.value(SETTING_EXPONENT_MIN).toInt());

        saveEnabledAddons(dialog.getEnabledAddons());
        saveSettings();
    }
}

void MainWindow::onActionExit() {
    saveSettings();
    addonManager->setActiveAddons({}); //Unload addons
    QCoreApplication::quit();
}

void MainWindow::onActionAbout() {
    auto *dialog = new AboutDialog(this);
    dialog->show();
    dialog->exec();
}

void MainWindow::onActionAboutQt() {
    QMessageBox::aboutQt(this);
}

void MainWindow::onActionOpenSymbolTable() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Import Symbols...");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);

    if (!dialog.exec()) {
        return;
    }

    QStringList list = dialog.selectedFiles();

    if (list.size() != 1) {
        return;
    }

    auto path = list[0].toStdString();
    if (importSymbolTable(path)) {
        symbolTablePathHistory.insert(path);
        saveSymbolTablePathHistory();
        updateSymbolHistoryMenu();
    }
}

void MainWindow::onActionSaveSymbolTable() {
    std::string filepath;
    if (currentSymbolTablePath.empty()) {
        onActionSaveAsSymbolTable();
    } else {
        saveSymbolTable(currentSymbolTablePath);
    }
}

void MainWindow::onActionSaveAsSymbolTable() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Symbols as ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    if (!dialog.exec()) {
        return;
    }

    QStringList list = dialog.selectedFiles();

    if (list.size() != 1) {
        return;
    }

    saveSymbolTable(list[0].toStdString());
}

void MainWindow::onActionEditSymbolTable() {
    if (symbolsDialog == nullptr) {
        symbolsDialog = new SymbolsDialog(symbolTable,
                                          this);
        connect(symbolsDialog,
                &QMainWindow::destroyed,
                [this]() {
                    symbolsDialog = nullptr;
                });
        connect(symbolsDialog,
                SIGNAL(symbolsChanged(const SymbolTable &)),
                this,
                SLOT(onSymbolTableChanged(const SymbolTable &)));
        symbolsDialog->show();
    } else {
        symbolsDialog->activateWindow();
    }
}

void MainWindow::onActionSymbolTableHistory() {
    importSymbolTable(dynamic_cast<QAction *>(sender())->data().toString().toStdString());
}

void MainWindow::onActionOpenTerminal() {
    auto *d = new TerminalWindow(this);
    d->setWindowTitle("Console");
    d->show();
}

const SymbolTable &MainWindow::getSymbolTable() {
    return symbolTable;
}

void MainWindow::onHistoryTextDoubleClicked(const QString &text) {
    const QString &currentText = input->text();
    input->setText(currentText + text);
    input->setFocus();
}

QString MainWindow::evaluateExpression(const QString &expression) {
    try {
        decimal::context.clear_status();
        auto v = ExpressionParser::evaluate(expression.toStdString(), symbolTable);
        if (settings.value(SETTING_WARN_INEXACT).toInt() && decimal::context.status() & MPD_Inexact) {
            QMessageBox::warning(this, "Inexact result",
                                 "Result is inexact, increase the precision to compute an exact result.");
        }
        onSymbolTableChanged(symbolTable);

        QString ret = v.format("f").c_str();
        emit signalExpressionEvaluated(expression, ret);
        return ret;
    } catch (const std::runtime_error &e) {
        QMessageBox::warning(this, "Failed to evaluate expression", e.what());
    }
    return "";
}

void MainWindow::loadSettings() {
    std::string settingsFilePath = Paths::getAppConfigDirectory().append(SETTINGS_FILE);
    if (QFile(settingsFilePath.c_str()).exists()) {
        try {
            settings = Serializer::deserializeSettings(FileOperations::fileReadAllText(settingsFilePath));
        } catch (const std::runtime_error &e) {
            QMessageBox::warning(this, "Failed to load settings", e.what());
            settings = {};
        }
    }

    if (settings.value(SETTING_PRECISION).toInt() < 0) {
        settings.clear(SETTING_PRECISION);
    }

    switch (settings.value(SETTING_ROUNDING).toInt()) {
        case MPD_ROUND_UP:
        case MPD_ROUND_DOWN:
        case MPD_ROUND_CEILING:
        case MPD_ROUND_FLOOR:
        case MPD_ROUND_HALF_UP:
        case MPD_ROUND_HALF_DOWN:
        case MPD_ROUND_HALF_EVEN:
        case MPD_ROUND_05UP:
        case MPD_ROUND_TRUNC:
        case MPD_ROUND_GUARD:
            break;
        default:
            settings.clear(SETTING_ROUNDING);
            break;
    }

    decimal::context.prec(settings.value(SETTING_PRECISION).toInt());
    decimal::context.round(settings.value(SETTING_ROUNDING).toInt());
    decimal::context.emax(settings.value(SETTING_EXPONENT_MAX).toInt());
    decimal::context.emin(settings.value(SETTING_EXPONENT_MIN).toInt());

    if (symbolsDialog != nullptr) {
        symbolsDialog->setSymbols(symbolTable);
    }
}

void MainWindow::saveSettings() {
    try {
        std::string dir = Paths::getAppConfigDirectory();

        if (!QDir(dir.c_str()).exists())
            QDir().mkpath(dir.c_str());

        FileOperations::fileWriteAllText(dir.append(SETTINGS_FILE), Serializer::serializeSettings(settings));
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save settings", e.what());
    }
}

std::set<std::string> MainWindow::loadEnabledAddons() {
    if (QFile(enabledAddonsFilePath).exists()) {
        try {
            return Serializer::deserializeSet(FileOperations::fileReadAllText(enabledAddonsFilePath.toStdString()));
        }
        catch (const std::runtime_error &e) {
            QMessageBox::warning(this, "Failed to load enabled addons", e.what());
            return {};
        }
    } else {
        return {};
    }
}

void MainWindow::saveEnabledAddons(const std::set<std::string> &addons) {
    try {
        std::string dataDir = Paths::getAppDataDirectory();

        if (!QDir(dataDir.c_str()).exists())
            QDir().mkpath(dataDir.c_str());

        FileOperations::fileWriteAllText(dataDir.append(ADDONS_FILE), Serializer::serializeSet(addons));
    }
    catch (const std::runtime_error &e) {
        QMessageBox::warning(this, "Failed to save enabled addons", e.what());
    }
}

void MainWindow::loadSymbolTablePathHistory() {
    std::string filePath = Paths::getAppDataDirectory().append(SYMBOL_TABLE_HISTORY_FILE);

    if (!std::filesystem::exists(filePath)) {
        symbolTablePathHistory = {};
        return;
    }

    try {
        symbolTablePathHistory = Serializer::deserializeSet(FileOperations::fileReadAllText(filePath));

        if (symbolTablePathHistory.size() > MAX_SYMBOL_TABLE_HISTORY) {
            std::set<std::string> tmp;
            int i = 0;
            for (auto it = symbolTablePathHistory.rbegin();
                 it != symbolTablePathHistory.rend() && i < MAX_SYMBOL_TABLE_HISTORY; it++, i++) {
                tmp.insert(it->c_str());
            }
            symbolTablePathHistory = tmp;
        }

        std::set<std::string> delPaths;
        for (const auto &path: symbolTablePathHistory) {
            if (!std::filesystem::exists(path))
                delPaths.insert(path);
        }

        for (const auto &path: delPaths)
            symbolTablePathHistory.erase(path);

    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to load symbol table history", e.what());
        symbolTablePathHistory = {};
    }
}

void MainWindow::saveSymbolTablePathHistory() {
    if (!settings.value(SETTING_SAVE_SYM_HISTORY).toInt()) {
        return;
    }

    try {
        std::string dataDir = Paths::getAppDataDirectory();

        if (!QDir(dataDir.c_str()).exists())
            QDir().mkpath(dataDir.c_str());

        FileOperations::fileWriteAllText(dataDir.append(SYMBOL_TABLE_HISTORY_FILE),
                                         Serializer::serializeSet(symbolTablePathHistory));
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save symbol table history", e.what());
    }
}

void MainWindow::setupMenuBar() {
    menuBar()->setObjectName("menubar");

    menuFile = new QMenu(this);
    menuFile->setObjectName("menuFile");
    menuFile->setTitle("File");

    menuSymbols = new QMenu(this);
    menuSymbols->setObjectName("menuSymbols");
    menuSymbols->setTitle("Symbols");

    menuHelp = new QMenu(this);
    menuHelp->setObjectName("menuHelp");
    menuHelp->setTitle("Help");

    menuOpenRecent = new QMenu(this);
    menuOpenRecent->setObjectName("menuOpenRecent");
    menuOpenRecent->setTitle("Open Recent");

    menuTools = new QMenu(this);
    menuTools->setObjectName("menuTools");
    menuTools->setTitle("Tools");

    actionOpenTerminal = new QAction(this);
    actionOpenTerminal->setText("Open Python Console");
    actionOpenTerminal->setObjectName("actionOpenTerminal");
    actionOpenTerminal->setShortcut(QKeySequence(Qt::CTRL + Qt::Key::Key_T));

    actionSettings = new QAction(this);
    actionSettings->setText("Settings");
    actionSettings->setObjectName("actionSettings");

    actionOpenSymbols = new QAction(this);
    actionOpenSymbols->setText("Open...");
    actionOpenSymbols->setObjectName("actionOpenSymbols");
    actionOpenSymbols->setShortcut(QKeySequence::Open);

    actionSaveSymbols = new QAction(this);
    actionSaveSymbols->setText("Save...");
    actionSaveSymbols->setObjectName("actionSaveSymbols");
    actionSaveSymbols->setShortcut(QKeySequence::Save);
    actionSaveSymbols->setEnabled(false);

    actionSaveAsSymbols = new QAction(this);
    actionSaveAsSymbols->setText("Save As...");
    actionSaveAsSymbols->setObjectName("actionSaveAsSymbols");
    actionSaveAsSymbols->setShortcut(QKeySequence::SaveAs);

    actionExit = new QAction(this);
    actionExit->setText("Exit");
    actionExit->setObjectName("actionExit");
    actionExit->setShortcut(QKeySequence::Quit);

    actionAbout = new QAction(this);
    actionAbout->setText("About QCalculator");
    actionAbout->setObjectName("actionAbout");

    actionAboutQt = new QAction(this);
    actionAboutQt->setText("About Qt");
    actionAboutQt->setObjectName("actionAboutQt");

    actionEditSymbols = new QAction(this);
    actionEditSymbols->setText("Edit");
    actionEditSymbols->setObjectName("actionEditSymbols");
    actionEditSymbols->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

    menuTools->addAction(actionOpenTerminal);

    menuFile->addAction(actionSettings);
    menuFile->addSeparator();
    menuFile->addAction(actionExit);

    menuSymbols->addAction(actionEditSymbols);
    menuSymbols->addSeparator();
    menuSymbols->addAction(actionOpenSymbols);
    menuSymbols->addMenu(menuOpenRecent);
    menuSymbols->addAction(actionSaveSymbols);
    menuSymbols->addAction(actionSaveAsSymbols);

    menuHelp->addAction(actionAbout);
    menuHelp->addAction(actionAboutQt);

    menuBar()->addMenu(menuFile);
    menuBar()->addMenu(menuSymbols);
    menuBar()->addMenu(menuTools);
    menuBar()->addMenu(menuHelp);
}

void MainWindow::setupLayout() {
    rootWidget = new QWidget(this);
    rootWidget->setObjectName("widget_root");

    history = new HistoryWidget(this);
    history->setObjectName("widget_history");

    input = new QLineEdit(this);
    input->setObjectName("lineEdit_input");

    auto l = new QVBoxLayout();

    l->addWidget(history);
    l->addWidget(input);

    for (int i = 0; i < 10; i++) {
        l->addSpacing(0);
    }

    rootWidget->setLayout(l);

    setCentralWidget(rootWidget);
}

void MainWindow::updateSymbolHistoryMenu() {
    auto menu = menuOpenRecent;
    menu->clear();
    for (auto rev = symbolTablePathHistory.rbegin(); rev != symbolTablePathHistory.rend(); rev++) {
        auto path = rev->c_str();
        auto action = menu->addAction(path);
        action->setData(path);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(onActionSymbolTableHistory()));
    }
}

bool MainWindow::importSymbolTable(const std::string &path) {
    try {
        auto syms = Serializer::deserializeTable(FileOperations::fileReadAllText(path));

        QMessageBox::information(this, "Import successful", ("Successfully imported symbols from " + path).c_str());

        std::set<std::string> addons = addonManager->getActiveAddons();
        addonManager->setActiveAddons({});

        symbolTable = syms;
        currentSymbolTablePath = path;

        actionSaveSymbols->setEnabled(true);

        if (symbolsDialog != nullptr) {
            symbolsDialog->setSymbols(symbolTable);
        }

        addonManager->setActiveAddons(addons);

        return true;
    } catch (const std::exception &e) {
        std::string error = "Failed to import symbols from ";
        error += path;
        error += " Error: ";
        error += e.what();
        QMessageBox::warning(this, "Import failed", error.c_str());

        return false;
    }
}

bool MainWindow::saveSymbolTable(const std::string &path) {
    try {
        FileOperations::fileWriteAllText(path, Serializer::serializeTable(symbolTable));

        symbolTablePathHistory.insert(path);
        saveSymbolTablePathHistory();
        updateSymbolHistoryMenu();

        currentSymbolTablePath = path;

        actionSaveSymbols->setEnabled(true);

        QMessageBox::information(this,
                                 "Export successful",
                                 ("Successfully saved symbols to " + path).c_str());
        return true;
    }
    catch (const std::exception &e) {
        std::string error = "Failed to save symbols to ";
        error += path;
        error += " Error: ";
        error += e.what();
        QMessageBox::warning(this, "Export failed", error.c_str());
        return false;
    }
}

