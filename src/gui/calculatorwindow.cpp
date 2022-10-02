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

#include "calculatorwindow.hpp"

#include <filesystem>

#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QApplication>
#include <QProcess>
#include <QProgressDialog>

#include "util/stringsplit.hpp"

#include "addon/addonmanager.hpp"

#include "io/paths.hpp"
#include "io/serializer.hpp"
#include "io/fileoperations.hpp"
#include "io/archive.hpp"

#include "settings/settingconstants.hpp"

#include "math/expressionparser.hpp"

#include "dialog/settings/settingsdialog.hpp"
#include "symbolseditorwindow.hpp"
#include "dialog/aboutdialog.hpp"

#include "widgets/historywidget.hpp"
#include "widgets/symbolseditor.hpp"

#include "pycx/modules/exprtkmodule.hpp"
#include "pycx/modules/stdredirmodule.hpp"

#include "pycx/interpreter.hpp"

#include "extern/exprtk_mpdecimal_adaptor.hpp"

#include "io/to_wstring.hpp"

static const int MAX_SYMBOL_TABLE_HISTORY = 100;
static const int MAX_HISTORY = 1000;

//TODO:Feature: Completion and history navigation for input line edit with eg. up / down arrows.
CalculatorWindow::CalculatorWindow(QWidget *parent) : QMainWindow(parent) {
    setObjectName("MainWindow");

    addonManager = std::make_unique<AddonManager>(Paths::getAddonDirectory(),
                                                  [this](const std::string &module, const std::string &error) {
                                                      return onAddonLoadFail(module, error);
                                                  },
                                                  [this](const std::string &module, const std::string &error) {
                                                      return onAddonUnloadFail(module, error);
                                                  });

    setupDialogs();
    setupLayout();
    setupMenuBar();

    input->setFocus();

    //Scale original point size by 1.3 and convert to integer (C++ Floating–integral conversion - fraction is truncated)
    QFont defaultFont = input->font();
    QFont largeFont(defaultFont.family(), (int) (defaultFont.pointSize() * 1.3));

    input->setFont(largeFont);
    historyWidget->setHistoryFont(largeFont);

    QPalette historyPalette = historyWidget->palette();
    historyPalette.setColor(historyWidget->backgroundRole(), input->palette().color(input->backgroundRole()));
    historyWidget->setPalette(historyPalette);

    connect(actionSettings, SIGNAL(triggered(bool)), this, SLOT(onActionSettings()));
    connect(actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExit()));
    connect(actionAbout, SIGNAL(triggered(bool)), this, SLOT(onActionAbout()));
    connect(actionAboutQt, SIGNAL(triggered(bool)), this, SLOT(onActionAboutQt()));
    connect(actionOpenSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionOpenSymbolTable()));
    connect(actionSaveSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionSaveSymbolTable()));
    connect(actionSaveAsSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionSaveAsSymbolTable()));
    connect(actionEditSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionEditSymbolTable()));
    connect(actionOpenTerminal, SIGNAL(triggered(bool)), this, SLOT(onActionOpenTerminal()));
    connect(actionExtractArchive, SIGNAL(triggered(bool)), this, SLOT(onActionExtractArchive()));
    connect(actionClearHistory, SIGNAL(triggered(bool)), this, SLOT(onActionClearHistory()));
    connect(actionAboutPython, SIGNAL(triggered(bool)), this, SLOT(onActionAboutPython()));

    connect(input, SIGNAL(returnPressed()), this, SLOT(onInputReturnPressed()));
    connect(input, SIGNAL(textChanged(const QString &)), this, SLOT(onInputTextChanged()));
    connect(input, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onInputCursorPositionChanged(int, int)));

    connect(historyWidget,
            SIGNAL(onTextDoubleClicked(const QString &)),
            this,
            SLOT(onHistoryTextDoubleClicked(const QString &)));

    loadSettings();

    loadSymbolTablePathHistory();
    saveSymbolTablePathHistory();

    updateSymbolHistoryMenu();

    for (auto &path: settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList()) {
        Interpreter::addModuleDir(path);
    }

    ExprtkModule::setGlobalTable(symbolTable,
                                 [this]() {
                                     onSymbolTableChanged(symbolTable);
                                 });

    enabledAddonsFilePath = Paths::getAddonsFile().c_str();

    std::set<std::string> enabledAddons = loadEnabledAddons();

    //Check for enabled addons which dont exist anymore.
    std::set<std::string> availableAddons;
    auto addons = addonManager->getAvailableAddons();
    for (auto &addon: enabledAddons) {
        if (addons.find(addon) != addons.end())
            availableAddons.insert(addon);
    }

    addonManager->setActiveAddons(availableAddons);
    addonManager->loadAddonLibraryPaths();

    settingsDialog->setEnabledAddons(addonManager->getActiveAddons());
}

CalculatorWindow::~CalculatorWindow() = default;

void CalculatorWindow::closeEvent(QCloseEvent *event) {
}

void CalculatorWindow::resizeEvent(QResizeEvent *event) {
}

void CalculatorWindow::onAddonLoadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to load module",
                         ("Module " + moduleName + " failed to load\n\n" + error).c_str());
}

void CalculatorWindow::onAddonUnloadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to unload module",
                         ("Module " + moduleName + " failed to unload\n\n" + error).c_str());
}

void CalculatorWindow::onEvaluateExpression(const QString &expression) {
    evaluateExpression(expression);
}

void CalculatorWindow::onInputReturnPressed() {
    auto expr = input->text();
    auto res = evaluateExpression(expr);
    if (!res.isEmpty()) {
        input->setText(res);
        historyWidget->addContent(expr, res);
        inputTextContainsExpressionResult = true;
    }
}

void CalculatorWindow::onInputTextChanged() {
    inputTextAppendedHistoryValue.clear();
    inputTextHistoryIndex = 0;

    if (inputTextContainsExpressionResult) {
        inputTextContainsExpressionResult = false;
        if (input->text().isEmpty())
            inputText = "";
        else
            inputText = input->text().toStdString().substr(inputText.size());
        input->setText(inputText.c_str());
    } else {
        inputText = input->text().toStdString();
    }
}

void CalculatorWindow::onSymbolTableChanged(const SymbolTable &symbolTableArg) {
    this->symbolTable = symbolTableArg;
    if (symbolsDialog != nullptr) {
        symbolsDialog->setSymbols(symbolTable);
    }
}

void CalculatorWindow::onActionSettings() {
    settingsDialog->show();
    settingsDialog->activateWindow();
}

void CalculatorWindow::onActionExit() {
    saveSettings();
    addonManager->setActiveAddons({}); //Unload addons
    QCoreApplication::quit();
}

void CalculatorWindow::onActionAbout() {
    auto *dialog = new AboutDialog(this);
    dialog->show();
    dialog->exec();
}

void CalculatorWindow::onActionAboutQt() {
    QMessageBox::aboutQt(this);
}

void CalculatorWindow::onActionAboutPython() {
    std::string version = Interpreter::getVersion();
    std::string copyright = Interpreter::getCopyright();

    std::string str = "Python version " + version + "\n\n" + copyright;

    QMessageBox::about(this, "About Python", str.c_str());
}

void CalculatorWindow::onActionOpenSymbolTable() {
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

void CalculatorWindow::onActionSaveSymbolTable() {
    std::string filepath;
    if (currentSymbolTablePath.empty()) {
        onActionSaveAsSymbolTable();
    } else {
        saveSymbolTable(currentSymbolTablePath);
    }
}

void CalculatorWindow::onActionSaveAsSymbolTable() {
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

void CalculatorWindow::onActionEditSymbolTable() {
    symbolsDialog->show();
    symbolsDialog->activateWindow();
}

void CalculatorWindow::onActionSymbolTableHistory() {
    importSymbolTable(dynamic_cast<QAction *>(sender())->data().toString().toStdString());
}

void CalculatorWindow::onActionOpenTerminal() {
    auto *d = new TerminalWindow(this);
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->setWindowTitle("Python Terminal");
    d->show();
}

void CalculatorWindow::onActionExtractArchive() {
    QFileDialog dialog;
    dialog.setWindowTitle("Select archive file...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);

    if (dialog.exec() == QFileDialog::Accepted) {
        auto archiveFile = dialog.selectedFiles().at(0);
        dialog.setWindowTitle("Select output directory...");
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFileMode(QFileDialog::Directory);

        if (dialog.exec() == QFileDialog::Accepted) {
            auto targetDirectory = dialog.selectedFiles()[0];
            try {
                Archive::extractToDisk(archiveFile.toStdString(),
                                       targetDirectory.toStdString(),
                                       [](const std::string &str) {});
                QMessageBox::information(this, "Extraction Successful",
                                         "Extracted " + archiveFile + " to " + targetDirectory);
            } catch (const std::exception &e) {
                QMessageBox::warning(this,
                                     "Extraction failed",
                                     e.what());
            }
        }
    }
}

const SymbolTable &CalculatorWindow::getSymbolTable() {
    return symbolTable;
}

void CalculatorWindow::onHistoryTextDoubleClicked(const QString &text) {
    input->setText(input->text() + text);
    input->setFocus();
}

void CalculatorWindow::onSettingsAccepted() {
    for (auto &path: settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList()) {
        Interpreter::removeModuleDir(path);
    }

    settings.update(SETTING_PRECISION.key, settingsDialog->getPrecision());
    settings.update(SETTING_EXPONENT_MAX.key, settingsDialog->getExponentMax());
    settings.update(SETTING_EXPONENT_MIN.key, settingsDialog->getExponentMin());
    settings.update(SETTING_ROUNDING.key, settingsDialog->getRoundingMode());
    settings.update(SETTING_WARN_INEXACT.key, settingsDialog->getShowInexactWarning());
    settings.update(SETTING_SAVE_HISTORY.key, settingsDialog->getSaveHistoryMax());

    settings.update(SETTING_PYTHON_MODULE_PATHS.key, settingsDialog->getPythonModPaths());
    settings.update(SETTING_PYTHON_PATH.key, settingsDialog->getPythonPath());

    saveSettings();
    saveHistory();

    auto str = settings.value(SETTING_PYTHON_PATH).toString();
    if (!str.empty()) {
        Interpreter::setPath(to_wstring(str));
    }
    for (auto &path: settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList()) {
        Interpreter::addModuleDir(path);
    }

    decimal::context.prec(settings.value(SETTING_PRECISION).toInt());
    decimal::context.round(settings.value(SETTING_ROUNDING).toInt());
    decimal::context.emax(settings.value(SETTING_EXPONENT_MAX).toInt());
    decimal::context.emin(settings.value(SETTING_EXPONENT_MIN).toInt());

    saveEnabledAddons(settingsDialog->getEnabledAddons());

    settingsDialog->hide();
}

void CalculatorWindow::onSettingsCancelled() {
    settingsDialog->hide();

    settingsDialog->setPrecision(settings.value(SETTING_PRECISION).toInt());
    settingsDialog->setExponentMin(settings.value(SETTING_EXPONENT_MIN).toInt());
    settingsDialog->setExponentMax(settings.value(SETTING_EXPONENT_MAX).toInt());
    settingsDialog->setRoundingMode(Serializer::deserializeRoundingMode(
            settings.value(SETTING_ROUNDING).toInt()));
    settingsDialog->setShowInexactWarning(settings.value(SETTING_WARN_INEXACT).toInt());
    settingsDialog->setSaveHistory(settings.value(SETTING_SAVE_HISTORY).toInt());

    settingsDialog->setPythonModPaths(settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList());
    settingsDialog->setPythonPath(settings.value(SETTING_PYTHON_PATH).toString());

    settingsDialog->setEnabledAddons(addonManager->getActiveAddons());
}

void CalculatorWindow::onActionClearHistory() {
    if (QMessageBox::question(this, "Clear History", "Do you want to clear the history?") == QMessageBox::Yes) {
        history.clear();
        historyWidget->clear();
    }
}

QString CalculatorWindow::evaluateExpression(const QString &expression) {
    try {
        decimal::context.clear_status();

        auto v = ExpressionParser::evaluate(expression.toStdString(), symbolTable);

        QString ret = v.format("f").c_str();

        history.emplace_back(std::make_pair(expression.toStdString(), ret.toStdString()));
        saveHistory();

        if (settings.value(SETTING_WARN_INEXACT).toInt() && decimal::context.status() & MPD_Inexact) {
            QMessageBox::warning(this, "Inexact result", "Result is inexact!");
        }
        onSymbolTableChanged(symbolTable);

        emit signalExpressionEvaluated(expression, ret);

        return ret;
    } catch (const std::runtime_error &e) {
        QMessageBox::warning(this, "Failed to evaluate expression", e.what());
    }
    return "";
}

void CalculatorWindow::loadSettings() {
    std::string settingsFilePath = Paths::getSettingsFile();
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

    settingsDialog->setPrecision(settings.value(SETTING_PRECISION).toInt());
    settingsDialog->setExponentMin(settings.value(SETTING_EXPONENT_MIN).toInt());
    settingsDialog->setExponentMax(settings.value(SETTING_EXPONENT_MAX).toInt());
    settingsDialog->setRoundingMode(Serializer::deserializeRoundingMode(
            settings.value(SETTING_ROUNDING).toInt()));
    settingsDialog->setShowInexactWarning(settings.value(SETTING_WARN_INEXACT).toInt());
    settingsDialog->setSaveHistory(settings.value(SETTING_SAVE_HISTORY).toInt());

    settingsDialog->setPythonModPaths(settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList());
    settingsDialog->setPythonPath(settings.value(SETTING_PYTHON_PATH).toString());

    loadHistory();
    saveHistory();
}

void CalculatorWindow::saveSettings() {
    try {
        FileOperations::fileWriteAllText(Paths::getSettingsFile(), Serializer::serializeSettings(settings));
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save settings", e.what());
    }
}

std::set<std::string> CalculatorWindow::loadEnabledAddons() {
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

void CalculatorWindow::saveEnabledAddons(const std::set<std::string> &addons) {
    try {
        FileOperations::fileWriteAllText(Paths::getAddonsFile(), Serializer::serializeSet(addons));
    }
    catch (const std::runtime_error &e) {
        QMessageBox::warning(this, "Failed to save enabled addons", e.what());
    }
}

void CalculatorWindow::loadSymbolTablePathHistory() {
    std::string filePath = Paths::getSymbolTableHistoryFile();

    if (!std::filesystem::exists(filePath)) {
        symbolTablePathHistory = {};
        return;
    }

    try {
        auto lines = splitString(FileOperations::fileReadAllText(filePath), '\n');
        for (auto &line: lines) {
            symbolTablePathHistory.insert(line);
        }

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

void CalculatorWindow::saveSymbolTablePathHistory() {
    if (!settings.value(SETTING_SAVE_SYMBOLS_HISTORY).toInt()) {
        return;
    }

    try {
        std::string str;
        for (auto &path: symbolTablePathHistory) {
            str += path + '\n';
        }
        str.pop_back();
        FileOperations::fileWriteAllText(Paths::getSymbolTableHistoryFile(), str);
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save symbol table history", e.what());
    }
}

void CalculatorWindow::setupMenuBar() {
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

    actionExtractArchive = new QAction(this);
    actionExtractArchive->setText("Extract archive...");
    actionExtractArchive->setObjectName("actionExtractArchive");

    actionClearHistory = new QAction(this);
    actionClearHistory->setText("Clear History");
    actionClearHistory->setObjectName("actionClearHistory");
    actionEditSymbols->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));

    actionAboutPython = new QAction(this);
    actionAboutPython->setText("About Python");
    actionAboutPython->setObjectName("actionAboutPython");

    menuTools->addAction(actionOpenTerminal);
    menuTools->addAction(actionExtractArchive);

    menuFile->addAction(actionSettings);
    menuFile->addSeparator();
    menuFile->addAction(actionClearHistory);
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
    menuHelp->addAction(actionAboutPython);

    menuBar()->addMenu(menuFile);
    menuBar()->addMenu(menuSymbols);
    menuBar()->addMenu(menuTools);
    menuBar()->addMenu(menuHelp);
}

void CalculatorWindow::setupLayout() {
    rootWidget = new QWidget(this);
    rootWidget->setObjectName("widget_root");

    historyWidget = new HistoryWidget(this);
    historyWidget->setObjectName("widget_history");

    input = new QLineEdit(this);
    input->setObjectName("lineEdit_input");

    auto l = new QVBoxLayout();

    l->addWidget(historyWidget);
    l->addWidget(input);

    for (int i = 0; i < 10; i++) {
        l->addSpacing(0);
    }

    rootWidget->setLayout(l);

    setCentralWidget(rootWidget);
}

void CalculatorWindow::setupDialogs() {
    symbolsDialog = new SymbolsEditorWindow(symbolTable,
                                            this);

    settingsDialog = new SettingsDialog(*addonManager, this);

    connect(symbolsDialog,
            SIGNAL(symbolsChanged(const SymbolTable &)),
            this,
            SLOT(onSymbolTableChanged(const SymbolTable &)));

    connect(settingsDialog,
            SIGNAL(accepted()),
            this,
            SLOT(onSettingsAccepted()));
    connect(settingsDialog,
            SIGNAL(rejected()),
            this,
            SLOT(onSettingsCancelled()));
}

void CalculatorWindow::updateSymbolHistoryMenu() {
    auto menu = menuOpenRecent;
    menu->clear();
    for (auto rev = symbolTablePathHistory.rbegin(); rev != symbolTablePathHistory.rend(); rev++) {
        auto path = rev->c_str();
        auto action = menu->addAction(path);
        action->setData(path);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(onActionSymbolTableHistory()));
    }
}

bool CalculatorWindow::importSymbolTable(const std::string &path) {
    try {
        auto syms = Serializer::deserializeTable(FileOperations::fileReadAllText(path));

        std::set<std::string> addons = addonManager->getActiveAddons();
        addonManager->setActiveAddons({});

        symbolTable = syms;
        currentSymbolTablePath = path;

        actionSaveSymbols->setEnabled(true);

        symbolsDialog->setSymbols(symbolTable);

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

bool CalculatorWindow::saveSymbolTable(const std::string &path) {
    try {
        FileOperations::fileWriteAllText(path, Serializer::serializeTable(symbolTable));

        symbolTablePathHistory.insert(path);
        saveSymbolTablePathHistory();
        updateSymbolHistoryMenu();

        currentSymbolTablePath = path;

        actionSaveSymbols->setEnabled(true);

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

void CalculatorWindow::saveHistory() {
    if (!settings.value(SETTING_SAVE_HISTORY).toInt() || history.empty())
        return;
    std::string outputStr;
    auto counter = 0;
    const auto counterMax = MAX_HISTORY;
    for (auto it = history.rbegin(); it != history.rend() && counter < counterMax; it++, counter++) {
        auto &pair = *it;
        std::string str = pair.first + "\n" + pair.second + "\n";
        outputStr += str;
    }
    FileOperations::fileWriteAllText(Paths::getHistoryFile(), outputStr);
}

void CalculatorWindow::loadHistory() {
    if (!settings.value(SETTING_SAVE_HISTORY).toInt())
        return;
    if (std::filesystem::exists(Paths::getHistoryFile())) {
        auto str = FileOperations::fileReadAllText(Paths::getHistoryFile());

        std::vector<std::string> lines = splitString(str, '\n');
        for (auto i = 0; lines.size() > 1 && i < lines.size() - 1; i += 2) {
            auto line = lines.at(i);
            auto nextLine = lines.at(i + 1);
            if (!line.empty() && !nextLine.empty()) {
                history.insert(history.begin(), std::make_pair(line, nextLine));
            }
        }

        historyWidget->clear();
        for (auto &pair: history) {
            historyWidget->addContent(pair.first.c_str(), pair.second.c_str());
        }
    }
}

void CalculatorWindow::onInputCursorPositionChanged(int oldPos, int newPos) {
    inputTextAppendedHistoryValue.clear();
    inputTextHistoryIndex = 0;
}

void CalculatorWindow::keyPressEvent(QKeyEvent *event) {
    if (input->hasFocus() && !history.empty()) {
        if (event->key() == Qt::Key_Up) {
            if (!inputTextAppendedHistoryValue.empty())
                inputTextHistoryIndex++;
        } else if (event->key() == Qt::Key_Down) {
            if (!inputTextAppendedHistoryValue.empty())
                inputTextHistoryIndex--;
        } else {
            QWidget::keyPressEvent(event);
            return;
        }

        if (inputTextHistoryIndex >= (int) history.size()) {
            inputTextHistoryIndex = 0;
        } else if (inputTextHistoryIndex < 0) {
            inputTextHistoryIndex = (int) history.size() - 1;
        }

        auto index = inputTextHistoryIndex;

        if (!inputTextAppendedHistoryValue.empty() && !input->text().isEmpty()) {
            auto cursor = input->cursorPosition();
            input->setText(input->text().remove(input->cursorPosition(),
                                                (int) inputTextAppendedHistoryValue.size()));
            input->setCursorPosition(cursor);
        }
        auto result = history.at((int) history.size() - index - 1).second;

        auto cursor = input->cursorPosition();
        auto inputStr = input->text().toStdString();
        auto inputBegin = inputStr.substr(0, cursor);
        auto inputEnd = inputStr.substr(cursor);
        if (cursor == 0)
            inputEnd = inputStr;
        input->setText((inputBegin + result + inputEnd).c_str());
        input->setCursorPosition(cursor);

        inputTextAppendedHistoryValue = result;
        inputTextHistoryIndex = index;
    }

    QWidget::keyPressEvent(event);
}
