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
#include "settingconstants.hpp"

#include "math/numberformat.hpp"
#include "math/expressionparser.hpp"

#include "dialog/settings/settingsdialog.hpp"
#include "dialog/symbolsdialog.hpp"
#include "dialog/aboutdialog.hpp"

#include "widgets/historywidget.hpp"
#include "widgets/symbolseditor.hpp"

#include "pycx/modules/exprtkmodule.hpp"
#include "pycx/modules/mprealmodule.hpp"
#include "pycx/modules/stdredirmodule.hpp"

#include "pycx/interpreter.hpp"

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

    connect(this,
            SIGNAL(signalInputTextChange(const QString &)),
            input,
            SLOT(setText(const QString &)));

    connect(this,
            SIGNAL(signalExpressionEvaluated(const QString &, const QString &)),
            history,
            SLOT(addContent(const QString &, const QString &)));

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

    std::string enabledAddonsFilePath = Paths::getAppDataDirectory().append(ADDONS_FILE);

    std::set<std::string> enabledAddons;
    if (QFile(enabledAddonsFilePath.c_str()).exists()) {
        try {
            enabledAddons = Serializer::deserializeSet(FileOperations::fileReadAllText(enabledAddonsFilePath));
        }
        catch (const std::runtime_error &e) {
            QMessageBox::warning(this, "Failed to load enabled addons", e.what());
            enabledAddons.clear();
        }
    }

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
    saveSettings();
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

void MainWindow::onInputReturnPressed() {
    try {
        std::string inputText = input->text().toStdString();

        ArithmeticType value = ExpressionParser::evaluate(inputText, symbolTable);

        // Apply side effects
        if (symbolsDialog != nullptr) {
            symbolsDialog->setSymbols(symbolTable);
        }

        std::string resultText = NumberFormat::toDecimal(value, settings.value(SETTING_KEY_FORMATTING_PRECISION,
                                                                               SETTING_DEFAULT_FORMATTING_PRECISION).toInt(),
                                                         Serializer::deserializeRoundingMode(
                                                                 settings.value(SETTING_KEY_FORMATTING_ROUNDING,
                                                                                SETTING_DEFAULT_FORMATTING_ROUNDING).toInt()));

        emit signalExpressionEvaluated(inputText.c_str(), resultText.c_str());

        inputText = resultText;

        emit signalInputTextChange(inputText.c_str());
    }
    catch (const std::runtime_error &e) {
        QMessageBox::warning(this, "Failed to evaluate expression", e.what());
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
    dialog.setPrecision(settings.value(SETTING_KEY_PRECISION, SETTING_DEFAULT_PRECISION).toInt());
    dialog.setRoundingMode(
            Serializer::deserializeRoundingMode(
                    settings.value(SETTING_KEY_ROUNDING, SETTING_DEFAULT_ROUNDING).toInt()));
    dialog.setFormattingPrecision(
            settings.value(SETTING_KEY_FORMATTING_PRECISION, SETTING_DEFAULT_FORMATTING_PRECISION).toInt());
    dialog.setFormattingRoundingMode(Serializer::deserializeRoundingMode(
            settings.value(SETTING_KEY_FORMATTING_ROUNDING, SETTING_DEFAULT_FORMATTING_ROUNDING).toInt()));
    dialog.setSymbolsPrecision(
            settings.value(SETTING_KEY_SYMBOLS_PRECISION, SETTING_DEFAULT_SYMBOLS_PRECISION).toInt());
    dialog.setSymbolsFormattingPrecision(settings.value(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION,
                                                        SETTING_DEFAULT_SYMBOLS_FORMATTING_PRECISION).toInt());
    dialog.setEnabledAddons(addonManager->getActiveAddons());

    dialog.show();

    if (dialog.exec() == QDialog::Accepted) {
        int precision = dialog.getPrecision();
        settings.setValue(SETTING_KEY_PRECISION, precision);

        mpfr_rnd_t rounding = dialog.getRoundingMode();
        settings.setValue(SETTING_KEY_ROUNDING, Serializer::serializeRoundingMode(rounding));

        int formattingPrecision = dialog.getFormattingPrecision();
        settings.setValue(SETTING_KEY_FORMATTING_PRECISION, formattingPrecision);

        mpfr_rnd_t formattingRounding = dialog.getFormattingRoundingMode();
        settings.setValue(SETTING_KEY_FORMATTING_ROUNDING, formattingRounding);

        int symbolsPrecision = dialog.getSymbolsPrecision();
        settings.setValue(SETTING_KEY_SYMBOLS_PRECISION, symbolsPrecision);

        int symbolsFormattingPrecision = dialog.getSymbolsFormattingPrecision();
        settings.setValue(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION, symbolsFormattingPrecision);

        mpfr::mpreal::set_default_prec(precision);
        mpfr::mpreal::set_default_rnd(rounding);

        if (symbolsDialog != nullptr) {
            symbolsDialog->setSymbolsPrecision(symbolsPrecision);
        }

        auto tmp = symbolTable.getVariables();
        for (auto &v: tmp) {
            v.second.setPrecision(symbolsPrecision, MPFR_RNDN);
            symbolTable.remove(v.first);
            symbolTable.setVariable(v.first, v.second);
        }

        tmp = symbolTable.getConstants();
        for (auto &v: tmp) {
            v.second.setPrecision(symbolsPrecision, MPFR_RNDN);
            symbolTable.remove(v.first);
            symbolTable.setConstant(v.first, v.second);
        }

        if (symbolsDialog != nullptr) {
            symbolsDialog->setSymbolsFormattingPrecision(symbolsFormattingPrecision);
        }

        try {
            std::set<std::string> addons = dialog.getEnabledAddons();
            std::string dataDir = Paths::getAppDataDirectory();

            if (!QDir(dataDir.c_str()).exists())
                QDir().mkpath(dataDir.c_str());

            FileOperations::fileWriteAllText(dataDir.append(ADDONS_FILE), Serializer::serializeSet(addons));
        }
        catch (const std::runtime_error &e) {
            QMessageBox::warning(this, "Failed to save enabled addons", e.what());
        }
    }
}

void MainWindow::onActionExit() {
    saveSettings();
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
                                          settings.value(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION,
                                                         SETTING_DEFAULT_SYMBOLS_FORMATTING_PRECISION).toInt(),
                                          settings.value(SETTING_KEY_SYMBOLS_PRECISION,
                                                         SETTING_DEFAULT_SYMBOLS_PRECISION).toInt(),
                                          this);
        connect(symbolsDialog,
                &QDialog::finished,
                [this](int) {
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
    auto *d = new TerminalDialog(this);
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

    int formattingPrecision = settings.value(SETTING_KEY_FORMATTING_PRECISION,
                                             SETTING_DEFAULT_FORMATTING_PRECISION).toInt();

    //Do bounds checking on the deserialized formatting precision
    if (formattingPrecision < 0 || formattingPrecision > MAX_FORMATTING_PRECISION) {
        formattingPrecision = 0;
        settings.setValue(SETTING_KEY_FORMATTING_PRECISION, formattingPrecision);
    }

    int symbolsFormattingPrecision = settings.value(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION,
                                                    SETTING_DEFAULT_SYMBOLS_FORMATTING_PRECISION).toInt();
    if (symbolsFormattingPrecision < 0 || symbolsFormattingPrecision > MAX_FORMATTING_PRECISION) {
        symbolsFormattingPrecision = 0;
        settings.setValue(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION, symbolsFormattingPrecision);
    }

    // Because on 32bit platforms long(mpfr_prec_t) and int can both be 32 bit and the mpfr documentation says
    // "no value near MPFR_PREC_MAX should be used" we limit the precision range
    // to a max of MPFR_PREC_MAX minus one third of MPFR_PREC_MAX (Which is redundant on 64bit).
    // The gui limits the ranges of these values, so this is to protect against invalid values in the settings file,
    // and should not affect normal user experience.
    int symbolsPrecision = settings.value(SETTING_KEY_SYMBOLS_PRECISION,
                                          SETTING_DEFAULT_SYMBOLS_PRECISION).toInt();
    if (symbolsPrecision < MPFR_PREC_MIN || (mpfr_prec_t) symbolsPrecision > (MPFR_PREC_MAX - (MPFR_PREC_MAX / 3))) {
        symbolsPrecision = MPFR_PREC_MIN;
        settings.setValue(SETTING_KEY_SYMBOLS_PRECISION, symbolsPrecision);
    }

    int precision = settings.value(SETTING_KEY_PRECISION, SETTING_DEFAULT_PRECISION).toInt();
    if (precision < MPFR_PREC_MIN || (mpfr_prec_t) precision > (MPFR_PREC_MAX - (MPFR_PREC_MAX / 3))) {
        precision = MPFR_PREC_MIN;
        settings.setValue(SETTING_KEY_PRECISION, precision);
    }

    mpfr_rnd_t rounding = Serializer::deserializeRoundingMode(
            settings.value(SETTING_KEY_ROUNDING, SETTING_DEFAULT_ROUNDING).toInt());

    mpfr::mpreal::set_default_prec(precision);
    mpfr::mpreal::set_default_rnd(rounding);

    if (symbolsDialog != nullptr) {
        symbolsDialog->setSymbolsPrecision(symbolsPrecision);
        symbolsDialog->setSymbols(symbolTable);
        symbolsDialog->setSymbolsFormattingPrecision(symbolsFormattingPrecision);
    }
}

void MainWindow::saveSettings() {
    addonManager->setActiveAddons({}); //Unload addons

    try {
        std::string dir = Paths::getAppConfigDirectory();

        if (!QDir(dir.c_str()).exists())
            QDir().mkpath(dir.c_str());

        FileOperations::fileWriteAllText(dir.append(SETTINGS_FILE), Serializer::serializeSettings(settings));
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save settings", e.what());
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
    if (!settings.value(SETTING_KEY_SAVE_SYM_HISTORY, SETTING_DEFAULT_SAVE_SYM_HISTORY).toInt()) {
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
    actionOpenTerminal->setText("Open Console");
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
        auto syms = Serializer::deserializeTable(FileOperations::fileReadAllText(path),
                                                 settings.value(SETTING_KEY_SYMBOLS_PRECISION,
                                                                SETTING_DEFAULT_SYMBOLS_PRECISION).toInt());

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
