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

#include "gui/mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>

#include "addon/addonmanager.hpp"
#include "addon/addonhelper.hpp"
#include "io/paths.hpp"
#include "io/serializer.hpp"
#include "io/fileoperations.hpp"
#include "settingconstants.hpp"

#include "math/numberformat.hpp"
#include "math/expressionparser.hpp"

#include "gui/settingsdialog.hpp"
#include "gui/widgets/historywidget.hpp"
#include "gui/widgets/symbolseditor.hpp"

#include "cpython/modules/exprtkmodule.hpp"
#include "cpython/pyutil.hpp"

#define ADDONS_FILE "/addons.json"
#define SETTINGS_FILE "/settings.json"

#define MAX_FORMATTING_PRECISION 100000

//TODO:Feature: Completion and history navigation for input line edit with eg. up / down arrows.
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow()) {
    ui->setupUi(this);

    history = new HistoryWidget(this);
    history->setObjectName("widget_history");

    ui->tab_history->layout()->addWidget(history);

    symbolsEditor = new SymbolsEditor(this);
    symbolsEditor->setObjectName("widget_symtable_editor");

    ui->tab_symbols->layout()->addWidget(symbolsEditor);

    ui->statusbar->hide();

    //Scale original point size by 1.3 and convert to integer (C++ Floating–integral conversion - fraction is truncated)
    QFont defaultFont = ui->lineEdit_input->font();
    QFont largeFont(defaultFont.family(), (int) (defaultFont.pointSize() * 1.3));

    ui->lineEdit_input->setFont(largeFont);
    history->setHistoryFont(largeFont);

    QPalette historyPalette = history->palette();
    historyPalette.setColor(history->backgroundRole(),
                            ui->lineEdit_input->palette().color(ui->lineEdit_input->backgroundRole()));
    history->setPalette(historyPalette);

    connect(symbolsEditor, SIGNAL(onSymbolsChanged(const SymbolTable &)), this,
            SLOT(onSymbolTableChanged(const SymbolTable &)));

    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(onActionSettings()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExit()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(onActionAbout()));
    connect(ui->actionImport_Symbols, SIGNAL(triggered(bool)), this, SLOT(onActionImportSymbolTable()));
    connect(ui->actionExport_Symbols, SIGNAL(triggered(bool)), this, SLOT(onActionExportSymbolTable()));

    connect(ui->lineEdit_input, SIGNAL(returnPressed()), this, SLOT(onInputReturnPressed()));

    connect(this,
            SIGNAL(signalInputTextChange(const QString &)),
            ui->lineEdit_input,
            SLOT(setText(const QString &)));

    connect(this,
            SIGNAL(signalExpressionEvaluated(const QString &, const QString &)),
            history,
            SLOT(addContent(const QString &, const QString &)));

    connect(history,
            SIGNAL(onTextDoubleClicked(const QString &)),
            this,
            SLOT(onHistoryTextDoubleClicked(const QString &)));

    std::string settingsFilePath = Paths::getAppDataDirectory().append(SETTINGS_FILE);
    if (QFile(settingsFilePath.c_str()).exists()) {
        try {
            settings = Serializer::deserializeSettings(FileOperations::fileReadAllText(settingsFilePath));
        }
        catch (const std::runtime_error &e) {
            QMessageBox::warning(this, "Failed to load settings", e.what());
            settings = {};
        }
    }

    resize(settings.value(SETTING_KEY_WINDOWSIZE_X, SETTING_DEFAULT_WINDOWSIZE_X).toInt(),
           settings.value(SETTING_KEY_WINDOWSIZE_Y, SETTING_DEFAULT_WINDOWSIZE_Y).toInt());

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

    symbolsEditor->setPrecision(symbolsPrecision);
    symbolsEditor->setSymbols(symbolTable, symbolsFormattingPrecision);

    ExprtkModule::initialize(*this);

    PyUtil::initializePython();
    PyUtil::addModuleDirectory(Paths::getSystemDirectory());
    PyUtil::addModuleDirectory(Paths::getAddonDirectory());

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
    auto addons = AddonHelper::getAvailableAddons(Paths::getAddonDirectory());
    for (auto &addon : enabledAddons) {
        if (addons.find(addon) != addons.end())
            availableAddons.insert(addon);
    }

    AddonManager::setActiveAddons(availableAddons, *this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    exitRoutine();
}

void MainWindow::resizeEvent(QResizeEvent *event) {
}

void MainWindow::onAddonLoadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to load module",
                         ("Module " + moduleName + " failed to load, Error: " + error).c_str());
}

void MainWindow::onAddonUnloadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to unload module",
                         ("Module " + moduleName + " failed to unload, Error: " + error).c_str());
}

void MainWindow::onInputReturnPressed() {
    try {
        std::string inputText = ui->lineEdit_input->text().toStdString();
        ArithmeticType value = ExpressionParser::evaluate(inputText, symbolTable);

        symbolsEditor->setSymbols(symbolTable, settings.value(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION,
                                                              SETTING_DEFAULT_SYMBOLS_FORMATTING_PRECISION).toInt());

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
    symbolsEditor->setSymbols(symbolTable, settings.value(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION,
                                                          SETTING_DEFAULT_SYMBOLS_FORMATTING_PRECISION).toInt());
}

void MainWindow::onActionSettings() {
    SettingsDialog dialog;
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
    dialog.setEnabledAddons(AddonManager::getActiveAddons());
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

        symbolsEditor->setPrecision(symbolsPrecision);

        auto tmp = symbolTable.getVariables();
        for (auto &v : tmp) {
            v.second.setPrecision(symbolsPrecision, MPFR_RNDN);
            symbolTable.remove(v.first);
            symbolTable.setVariable(v.first, v.second);
        }

        tmp = symbolTable.getConstants();
        for (auto &v : tmp) {
            v.second.setPrecision(symbolsPrecision, MPFR_RNDN);
            symbolTable.remove(v.first);
            symbolTable.setConstant(v.first, v.second);
        }

        symbolsEditor->setSymbols(symbolTable, symbolsFormattingPrecision);

        std::set<std::string> addons = dialog.getEnabledAddons();

        AddonManager::setActiveAddons(addons, *this);

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
}

void MainWindow::onActionExit() {
    exitRoutine();
    QCoreApplication::quit();
}

void MainWindow::onActionAbout() {
    QMessageBox::about(this, "About " + QApplication::applicationDisplayName(),
                       QApplication::applicationDisplayName()
                       + " "
                       + QApplication::applicationVersion()
                       + "\n\nCopyright (c) 2021 Julian Zampiccoli"
                       + "\n\n"
                       + QString(R"LLL(Source:
    https://github.com/xenotux/qcalc

Powered by:
    exprtk - https://github.com/ArashPartow/exprtk
    cpython - https://github.com/python/cpython
    qt - https://www.qt.io/
    mpfr - https://www.mpfr.org/
)LLL"));
}

void MainWindow::onActionImportSymbolTable() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Import Symbols...");
    dialog.setFileMode(QFileDialog::ExistingFile);

    if (!dialog.exec()) {
        return;
    }

    QStringList list = dialog.selectedFiles();

    if (list.size() != 1) {
        return;
    }

    std::string filepath = list[0].toStdString();

    std::set<std::string> addons = AddonManager::getActiveAddons();
    AddonManager::setActiveAddons({}, *this);

    try {
        symbolTable = Serializer::deserializeTable(FileOperations::fileReadAllText(filepath),
                                                   settings.value(SETTING_KEY_SYMBOLS_PRECISION,
                                                                  SETTING_DEFAULT_SYMBOLS_PRECISION).toInt());
        symbolsEditor->setSymbols(symbolTable, settings.value(SETTING_KEY_SYMBOLS_FORMATTING_PRECISION,
                                                              SETTING_DEFAULT_SYMBOLS_FORMATTING_PRECISION).toInt());
        QMessageBox::information(this, "Import successful", ("Successfully imported symbols from " + filepath).c_str());
    }
    catch (const std::exception &e) {
        std::string error = "Failed to import symbols from ";
        error += filepath;
        error += " Error: ";
        error += e.what();
        QMessageBox::warning(this, "Import failed", error.c_str());
    }

    AddonManager::setActiveAddons(addons, *this);
}

void MainWindow::onActionExportSymbolTable() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Import Symbols...");
    dialog.setFileMode(QFileDialog::AnyFile);

    if (!dialog.exec()) {
        return;
    }

    QStringList list = dialog.selectedFiles();

    if (list.size() != 1) {
        return;
    }

    std::string filepath = list[0].toStdString();

    try {
        FileOperations::fileWriteAllText(filepath, Serializer::serializeTable(symbolTable));
        QMessageBox::information(this,
                                 "Export successful",
                                 ("Successfully exported symbols to " + filepath).c_str());
    }
    catch (const std::exception &e) {
        std::string error = "Failed to export symbols to ";
        error += filepath;
        error += " Error: ";
        error += e.what();
        QMessageBox::warning(this, "Export failed", error.c_str());
    }
}

const SymbolTable &MainWindow::getSymbolTable() {
    return symbolTable;
}

void MainWindow::onHistoryTextDoubleClicked(const QString &text) {
    const QString &currentText = ui->lineEdit_input->text();
    ui->lineEdit_input->setText(currentText + text);
    ui->lineEdit_input->setFocus();
}

void MainWindow::exitRoutine() {
    AddonManager::setActiveAddons({}, *this);

    settings.setValue(SETTING_KEY_WINDOWSIZE_X, size().width());
    settings.setValue(SETTING_KEY_WINDOWSIZE_Y, size().height());

    try {
        std::string dataDir = Paths::getAppDataDirectory();

        if (!QDir(dataDir.c_str()).exists())
            QDir().mkpath(dataDir.c_str());

        FileOperations::fileWriteAllText(dataDir.append(SETTINGS_FILE),
                                         Serializer::serializeSettings(settings));
    }
    catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save settings", e.what());
    }
}
