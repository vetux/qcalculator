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

#include "windows/calculatorwindow.hpp"

#include <filesystem>
#include <string>
#include <cstdlib>

#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QMenuBar>
#include <QApplication>
#include <QInputDialog>
#include <QCompleter>

#include "addon/addonmanager.hpp"

#include "io/paths.hpp"
#include "io/serializer.hpp"
#include "io/fileoperations.hpp"
#include "io/archive.hpp"
#include "io/stringutil.hpp"

#include "settings/settingconstants.hpp"

#include "math/expressionparser.hpp"

#include "windows/settingsdialog.hpp"
#include "windows/symbolseditorwindow.hpp"
#include "windows/aboutdialog.hpp"

#include "widgets/historywidget.hpp"

#include "pycx/modules/exprtkmodule.hpp"
#include "pycx/interpreter.hpp"

static const int MAX_SYMBOL_TABLE_HISTORY = 100;
static const int MAX_HISTORY = 1000;

CalculatorWindow::CalculatorWindow(const QString &initErrorMessage, QWidget *parent) : QMainWindow(parent) {
    setObjectName("MainWindow");

    addonManager = std::make_unique<AddonManager>(Paths::getAddonDirectory(),
                                                  [this](const std::string &module, const std::string &error) {
                                                      return onAddonLoadFail(module, error);
                                                  },
                                                  [this](const std::string &module, const std::string &error) {
                                                      return onAddonUnloadFail(module, error);
                                                  });

    setupMenuBar();
    setupDialogs();
    setupLayout();

    input->setFocus();

    //Scale original point size by 1.3 and convert to integer (C++ Floating–integral conversion - fraction is truncated)
    QFont defaultFont = input->font();
    QFont largeFont(defaultFont.family(), (int) (defaultFont.pointSize() * 1.3));

    input->setFont(largeFont);
    historyWidget->setHistoryFont(largeFont);

    QPalette historyPalette = historyWidget->palette();
    historyPalette.setColor(historyWidget->backgroundRole(), input->palette().color(input->backgroundRole()));
    historyWidget->setPalette(historyPalette);

    connect(actions.actionSettings, SIGNAL(triggered(bool)), this, SLOT(onActionSettings()));
    connect(actions.actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExit()));
    connect(actions.actionAbout, SIGNAL(triggered(bool)), this, SLOT(onActionAbout()));
    connect(actions.actionAboutQt, SIGNAL(triggered(bool)), this, SLOT(onActionAboutQt()));
    connect(actions.actionClearSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionClearSymbolTable()));
    connect(actions.actionOpenSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionOpenSymbolTable()));
    connect(actions.actionSaveSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionSaveSymbolTable()));
    connect(actions.actionSaveAsSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionSaveAsSymbolTable()));
    connect(actions.actionEditSymbols, SIGNAL(triggered(bool)), this, SLOT(onActionEditSymbolTable()));
    connect(actions.actionOpenTerminal, SIGNAL(triggered(bool)), this, SLOT(onActionOpenTerminal()));
    connect(actions.actionCompressDirectory, SIGNAL(triggered(bool)), this, SLOT(onActionCompressDirectory()));
    connect(actions.actionExtractArchive, SIGNAL(triggered(bool)), this, SLOT(onActionExtractArchive()));
    connect(actions.actionCreateAddonBundle, SIGNAL(triggered(bool)), this, SLOT(onActionCreateAddonBundle()));
    connect(actions.actionClearHistory, SIGNAL(triggered(bool)), this, SLOT(onActionClearHistory()));
    connect(actions.actionAboutPython, SIGNAL(triggered(bool)), this, SLOT(onActionAboutPython()));

    connect(input, SIGNAL(returnPressed()), this, SLOT(onInputReturnPressed()));
    connect(input, SIGNAL(textChanged(const QString &)), this, SLOT(onInputTextChanged()));
    connect(input, SIGNAL(textEdited(const QString &)), this, SLOT(onInputTextEdited()));
    connect(input, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(onInputCursorPositionChanged(int, int)));

    connect(historyWidget,
            SIGNAL(onTextDoubleClicked(const QString &)),
            this,
            SLOT(onHistoryTextDoubleClicked(const QString &)));

    loadSettings();

    loadSymbolTablePathHistory();
    saveSymbolTablePathHistory();

    updateSymbolHistoryMenu();

    if (Interpreter::isInitialized()) {
        Interpreter::setStdStreams([this](const std::string &str) {
                                       terminalDialog->printOutput(str.c_str());
                                   },
                                   [this](const std::string &str) {
                                       terminalDialog->printError(str.c_str());
                                       terminalDialog->show();
                                       terminalDialog->activateWindow();
                                   });

        for (auto &path: settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList()) {
            Interpreter::addModuleDir(path);
        }
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
    addonManager->setAddonLibraryPaths();

    settingsDialog->setEnabledAddons(addonManager->getActiveAddons());

    setWindowIcon(QIcon(Paths::getCalculatorIconFile().c_str()));

    if (initErrorMessage.isEmpty()) {
        terminalDialog->printOutput("Initialized Python " + QString(Interpreter::getVersion().c_str()));
    } else {
        terminalDialog->printError("Failed to initialize Python:\n" + initErrorMessage);
    }
}

CalculatorWindow::~CalculatorWindow() {}

void CalculatorWindow::closeEvent(QCloseEvent *event) {
    cleanupDialogs();
}

void CalculatorWindow::resizeEvent(QResizeEvent *event) {}

void CalculatorWindow::onAddonLoadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to load module",
                         ("Module " + moduleName + " failed to load\n\n" + error).c_str());
}

void CalculatorWindow::onAddonUnloadFail(const std::string &moduleName, const std::string &error) {
    QMessageBox::warning(this, "Failed to unload module",
                         ("Module " + moduleName + " failed to unload\n\n" + error).c_str());
}

void CalculatorWindow::onInputReturnPressed() {
    if (completerWord.isEmpty()) {
        inputTextContainsExpressionResult = false;
        auto expr = input->text();
        auto res = evaluateExpression(expr);
        if (!res.isEmpty()) {
            input->setText(res);
            historyWidget->addContent(expr, res);
            inputTextContainsExpressionResult = true;
            previousResult = res.toStdString();
            if (decimal::context.status() & MPD_Inexact) {
                inputMessage->setText("Inexact");
            }
        }
    }
}

void CalculatorWindow::onInputTextChanged() {
    if (inputTextContainsExpressionResult) {
        inputTextContainsExpressionResult = false;
    }
    inputMessage->setText("");
}

void CalculatorWindow::onInputTextEdited() {
    clearResultFromInputText();

    auto cursorPos = input->cursorPosition();
    auto text = input->text().toStdString();
    auto wordStart = text.rfind(' ', cursorPos - 1) + 1;
    if (wordStart == std::string::npos)
        wordStart = 0;
    auto word = text.substr(wordStart, cursorPos - wordStart);

    bool found = false;

    QStringList contents;
    for (auto &var: symbolTable.getVariables()) {
        contents.append(var.first.c_str());
        if (!found && var.first.find(word) == 0)
            found = true;
    }
    for (auto &var: symbolTable.getConstants()) {
        contents.append(var.first.c_str());
        if (!found && var.first.find(word) == 0)
            found = true;
    }
    for (auto &var: symbolTable.getFunctions()) {
        contents.append(var.first.c_str());
        if (!found && var.first.find(word) == 0)
            found = true;
    }
    for (auto &var: symbolTable.getScripts()) {
        contents.append(var.first.c_str());
        if (!found && var.first.find(word) == 0)
            found = true;
    }
    contents.append("pi");
    contents.append("epsilon");
    contents.append("inf");
    for (auto &var: SymbolTable::getBuiltIns()) {
        contents.append(var.first.c_str());
        if (!found && var.first.find(word) == 0)
            found = true;
    }

    if (found
        && !text.empty()
        && !word.empty()
        && (cursorPos >= text.size()
            || text.at(cursorPos) == ' ')) {
        completerWord = QString(word.c_str());

        if (completer == nullptr) {
            completer = new QCompleter(contents, input);
            completer->setWidget(input);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            completer->setCompletionMode(QCompleter::PopupCompletion);

            connect(completer,
                    SIGNAL(activated(const QString &)),
                    this,
                    SLOT(insertInputText(const QString &)));
        }

        completer->setCompletionPrefix(completerWord);
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0, 0));
        completer->complete();
    } else {
        completerWord = "";
        if (completer != nullptr) {
            completer->popup()->hide();
            completer->disconnect(this);
            completer = nullptr;
        }
    }
}

void CalculatorWindow::onSymbolTableChanged(const SymbolTable &symbolTableArg) {
    this->symbolTable = symbolTableArg;
    symbolsDialog->setSymbols(symbolTable);
    if (!currentSymbolTablePath.empty()) {
        actions.actionSaveSymbols->setEnabled(true);
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

    QMessageBox box;
    box.setWindowTitle("About Python");
    box.setText(str.c_str());
    box.exec();
}

void CalculatorWindow::onActionClearSymbolTable() {
    if (QMessageBox::question(this, "Clear symbols", "Do you want to clear the symbol table?") == QMessageBox::Yes) {
        auto symCopy = symbolTable;
        symCopy.clearVariables();
        symCopy.clearConstants();
        symCopy.clearFunctions();
        onSymbolTableChanged(symCopy);
    }
}

void CalculatorWindow::onActionOpenSymbolTable() {
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open symbol table...");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters({"application/json"});

    if (!dialog.exec()) {
        return;
    }

    QStringList list = dialog.selectedFiles();

    if (list.size() != 1) {
        return;
    }

    auto path = list[0].toStdString();
    if (loadSymbolTable(path)) {
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
    dialog.setWindowTitle("Save symbol table as ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setMimeTypeFilters({"application/json"});

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
    loadSymbolTable(dynamic_cast<QAction *>(sender())->data().toString().toStdString());
}

void CalculatorWindow::onActionOpenTerminal() {
    terminalDialog->show();
    terminalDialog->activateWindow();
}

void CalculatorWindow::onActionCompressDirectory() {
    QFileDialog dialog;
    dialog.setWindowTitle("Select directory to compress...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::Directory);

    if (dialog.exec() == QFileDialog::Accepted) {
        auto directory = dialog.selectedFiles().at(0);
        auto directoryBase = std::filesystem::path(directory.toStdString()).parent_path().string();

        dialog.setWindowTitle("Select output file...");
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setMimeTypeFilters(Archive::getFormatMimeTypes());

        for (;;) {
            if (dialog.exec() == QFileDialog::Accepted) {
                auto outputFile = dialog.selectedFiles().at(0);

                Archive archive;
                auto files = FileOperations::findFilesInDirectory(directory.toStdString(), "", true);
                for (auto &file: files) {
                    auto name = file.substr(directoryBase.size() + 1);
                    archive.addEntry(name, FileOperations::fileReadAllVector(file));
                }

                try {
                    auto format = Archive::getFormatFromExtension(
                            std::filesystem::path(outputFile.toStdString()).extension().string());
                    archive.save(outputFile.toStdString(), format);
                    QMessageBox::information(this, "Compression successful",
                                             "Successfully compressed " + directory + " to " + outputFile);
                    break;
                } catch (const std::exception &e) {
                    QMessageBox::warning(this, "Compression failed", "Failed to save file: " + QString(e.what()));
                }
            } else {
                QMessageBox::information(this, "Compression cancelled", "The compression has been cancelled.");
                break;
            }
        }
    } else {
        QMessageBox::information(this, "Compression cancelled", "The compression has been cancelled.");
    }
}

void CalculatorWindow::onActionExtractArchive() {
    QFileDialog dialog;
    dialog.setWindowTitle("Select archive file...");
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setMimeTypeFilters(Archive::getFormatMimeTypes());

    if (dialog.exec() == QFileDialog::Accepted) {
        auto archiveFile = dialog.selectedFiles().at(0);
        dialog.setWindowTitle("Select output directory...");
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setMimeTypeFilters({});

        if (dialog.exec() == QFileDialog::Accepted) {
            auto targetDirectory = dialog.selectedFiles()[0];
            try {
                Archive::extractToDisk(archiveFile.toStdString(),
                                       targetDirectory.toStdString(),
                                       [](const std::string &str) {});
                QMessageBox::information(this, "Extraction successful",
                                         "Successfully extracted " + archiveFile + " to " + targetDirectory);
            } catch (const std::exception &e) {
                QMessageBox::warning(this,
                                     "Extraction failed",
                                     e.what());
            }
        } else {
            QMessageBox::information(this, "Extraction cancelled", "The extraction has been cancelled.");
        }
    } else {
        QMessageBox::information(this, "Extraction cancelled", "The extraction has been cancelled.");
    }
}

void CalculatorWindow::onActionCreateAddonBundle() {
    QFileDialog dialog;

    std::vector<AddonManager::InstallBundleEntry> bundleEntries;

    for (;;) {
        dialog.setWindowTitle("Select addon module file...");
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setOption(QFileDialog::ShowDirsOnly, false);
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setMimeTypeFilters({"text/x-python"});

        if (dialog.exec() == QFileDialog::Accepted) {
            auto moduleStr = dialog.selectedFiles().at(0).toStdString();
            auto modulePath = std::filesystem::path(moduleStr);
            if (modulePath.extension().string() != ".py") {
                QMessageBox::information(this, "Invalid selection",
                                         "The selected path does not point to a python module. Select a file ending in .py");
                continue;
            }

            QMessageBox::information(this, "Addon module added", ("Added " + modulePath.stem().string()).c_str());

            std::set<std::string> packages;
            for (;;) {
                auto r = QMessageBox::question(this, "Add package",
                                               "Do you want to add a package?",
                                               "Add Directory",
                                               "Add File",
                                               "Continue");
                if (r == 0) {
                    dialog.setWindowTitle("Select python package directory...");
                    dialog.setFileMode(QFileDialog::Directory);
                    dialog.setOption(QFileDialog::ShowDirsOnly, true);
                    dialog.setMimeTypeFilters({});
                } else if (r == 1) {
                    dialog.setWindowTitle("Select python package file...");
                    dialog.setFileMode(QFileDialog::AnyFile);
                    dialog.setOption(QFileDialog::ShowDirsOnly, false);
                    dialog.setMimeTypeFilters(Archive::getFormatMimeTypes());
                } else if (r == 2) {
                    break;
                }
                if (dialog.exec() == QFileDialog::Accepted) {
                    auto packagePath = dialog.selectedFiles().at(0);
                    packages.insert(packagePath.toStdString());
                    QMessageBox::information(this, "Added package", "Added " + QString(std::filesystem::path(
                            packagePath.toStdString()).stem().string().c_str()) + " to " +
                                                                    QString(modulePath.stem().string().c_str()));
                }
            }

            QInputDialog inputDialog;
            inputDialog.setWindowTitle("Set addon version");
            inputDialog.setLabelText("Select the addon version");
            int version = 0;
            if (inputDialog.exec() == QInputDialog::Accepted) {
                version = inputDialog.intValue();
            }

            AddonManager::InstallBundleEntry entry;
            entry.module = moduleStr;
            entry.packages = packages;
            entry.version = version;

            bundleEntries.emplace_back(entry);
        } else {
            break;
        }

        if (QMessageBox::question(this, "Add addon", "Do you want to add more addons?") == QMessageBox::No) {
            break;
        }
    }

    if (bundleEntries.empty()) {
        QMessageBox::information(this,
                                 "No selection",
                                 "No addons were selected");
        return;
    }

    dialog.setWindowTitle("Select output file...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setOption(QFileDialog::ShowDirsOnly, false);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setMimeTypeFilters({Archive::getFormatMimeTypes()});

    for (;;) {
        if (dialog.exec() == QFileDialog::Accepted) {
            auto outputFile = dialog.selectedFiles().at(0);
            auto outputPath = std::filesystem::path(outputFile.toStdString());
            try {
                auto format = Archive::getFormatFromExtension(outputPath.extension().string());
                auto archive = AddonManager::createInstallableBundle(bundleEntries);
                archive.save(outputFile.toStdString(), format);
                QMessageBox::information(this,
                                         "Saving successful",
                                         "Successfully saved addon bundle to " + outputFile);
                break;
            } catch (std::runtime_error &e) {
                QMessageBox::warning(this,
                                     "Saving failed",
                                     (std::string("Failed to save addon bundle: ") + e.what()).c_str());
            }
        } else {
            QMessageBox::information(this, "Cancelled saving", "Saving was cancelled.");
            break;
        }
    }
}

const SymbolTable &CalculatorWindow::getSymbolTable() {
    return symbolTable;
}

void CalculatorWindow::onHistoryTextDoubleClicked(const QString &text) {
    clearResultFromInputText();
    auto cursor = input->cursorPosition();
    auto fullText = input->text().toStdString();
    auto textBegin = QString(fullText.substr(0, cursor).c_str());
    auto textEnd = QString(fullText.substr(cursor).c_str());
    input->setText(textBegin + text + textEnd);
    input->setFocus();
    input->setCursorPosition(cursor + text.size());
}

void CalculatorWindow::onSettingsAccepted() {
    if (Interpreter::isInitialized()) {
        for (auto &path: settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList()) {
            Interpreter::removeModuleDir(path);
        }
    }

    settings.update(SETTING_PRECISION.key, settingsDialog->getPrecision());
    settings.update(SETTING_EXPONENT_MAX.key, settingsDialog->getExponentMax());
    settings.update(SETTING_EXPONENT_MIN.key, settingsDialog->getExponentMin());
    settings.update(SETTING_ROUNDING.key, settingsDialog->getRoundingMode());
    settings.update(SETTING_SAVE_HISTORY.key, settingsDialog->getSaveHistoryMax());

    settings.update(SETTING_PYTHON_MODULE_PATHS.key, settingsDialog->getPythonModPaths());
    settings.update(SETTING_PYTHON_PATH.key, settingsDialog->getPythonPath());

    saveSettings();
    saveHistory();

    if (Interpreter::isInitialized()) {
        auto str = settings.value(SETTING_PYTHON_PATH).toString();
        if (!str.empty()) {
            Interpreter::setPath(StringUtil::to_wstring(str));
        }
        for (auto &path: settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList()) {
            Interpreter::addModuleDir(path);
        }
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

void CalculatorWindow::insertInputText(const QString &v) {
    auto cursor = input->cursorPosition() - completerWord.size();
    auto text = input->text().toStdString();
    auto textBegin = text.substr(0, cursor);
    auto textEnd = text.substr(input->cursorPosition());
    input->setText(QString(textBegin.c_str()) + v + QString(textEnd.c_str()));
    input->setCursorPosition(cursor + v.size());
    completer->popup()->hide();
    completer = nullptr;
    completerWord = "";
}

QString CalculatorWindow::evaluateExpression(const QString &expression) {
    try {
        decimal::context.clear_status();

        auto v = ExpressionParser::evaluate(expression.toStdString(), symbolTable);

        QString ret = v.format("f").c_str();

        history.emplace_back(std::make_pair(expression.toStdString(), ret.toStdString()));
        saveHistory();

        onSymbolTableChanged(symbolTable);

        emit signalExpressionEvaluated(expression, ret);

        input->update();

        return ret;
    } catch (const std::exception &e) {
        inputMessage->setText(e.what());
    }
    return "";
}

void CalculatorWindow::loadSettings() {
    std::string settingsFilePath = Paths::getSettingsFile();
    if (QFile(settingsFilePath.c_str()).exists()) {
        try {
            settings = Serializer::deserializeSettings(FileOperations::fileReadAll(settingsFilePath));
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

    symbolsDialog->setSymbols(symbolTable);

    settingsDialog->setPrecision(settings.value(SETTING_PRECISION).toInt());
    settingsDialog->setExponentMin(settings.value(SETTING_EXPONENT_MIN).toInt());
    settingsDialog->setExponentMax(settings.value(SETTING_EXPONENT_MAX).toInt());
    settingsDialog->setRoundingMode(Serializer::deserializeRoundingMode(
            settings.value(SETTING_ROUNDING).toInt()));
    settingsDialog->setSaveHistory(settings.value(SETTING_SAVE_HISTORY).toInt());

    settingsDialog->setPythonModPaths(settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList());
    settingsDialog->setPythonPath(settings.value(SETTING_PYTHON_PATH).toString());

    loadHistory();
    saveHistory();
}

void CalculatorWindow::saveSettings() {
    try {
        FileOperations::fileWriteAll(Paths::getSettingsFile(), Serializer::serializeSettings(settings));
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save settings", e.what());
    }
}

std::set<std::string> CalculatorWindow::loadEnabledAddons() {
    if (QFile(enabledAddonsFilePath).exists()) {
        try {
            return Serializer::deserializeSet(FileOperations::fileReadAll(enabledAddonsFilePath.toStdString()));
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
        FileOperations::fileWriteAll(Paths::getAddonsFile(), Serializer::serializeSet(addons));
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
        auto lines = StringUtil::splitString(FileOperations::fileReadAll(filePath), '\n');
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
        FileOperations::fileWriteAll(Paths::getSymbolTableHistoryFile(), str);
    } catch (const std::exception &e) {
        QMessageBox::warning(this, "Failed to save symbol table history", e.what());
    }
}

void CalculatorWindow::setupMenuBar() {
    menuBar()->setObjectName("menubar");

    actions.menuFile = new QMenu(this);
    actions.menuFile->setObjectName("actions.menuFile");
    actions.menuFile->setTitle("File");

    actions.menuSymbols = new QMenu(this);
    actions.menuSymbols->setObjectName("actions.menuSymbols");
    actions.menuSymbols->setTitle("Symbols");

    actions.menuHelp = new QMenu(this);
    actions.menuHelp->setObjectName("actions.menuHelp");
    actions.menuHelp->setTitle("Help");

    actions.menuOpenRecent = new QMenu(this);
    actions.menuOpenRecent->setObjectName("actions.menuOpenRecent");
    actions.menuOpenRecent->setTitle("Open Recent");

    actions.menuTools = new QMenu(this);
    actions.menuTools->setObjectName("actions.menuTools");
    actions.menuTools->setTitle("Tools");

    actions.actionOpenTerminal = new QAction(this);
    actions.actionOpenTerminal->setText("Open Python Console");
    actions.actionOpenTerminal->setObjectName("actions.actionOpenTerminal");
    actions.actionOpenTerminal->setShortcut(QKeySequence(Qt::CTRL + Qt::Key::Key_T));

    actions.actionSettings = new QAction(this);
    actions.actionSettings->setText("Settings");
    actions.actionSettings->setObjectName("actions.actionSettings");

    actions.actionClearSymbols = new QAction(this);
    actions.actionClearSymbols->setText("Clear Symbols");
    actions.actionClearSymbols->setObjectName("actions.actionClearSymbols");

    actions.actionOpenSymbols = new QAction(this);
    actions.actionOpenSymbols->setText("Open...");
    actions.actionOpenSymbols->setObjectName("actions.actionOpenSymbols");
    actions.actionOpenSymbols->setShortcut(QKeySequence::Open);

    actions.actionSaveSymbols = new QAction(this);
    actions.actionSaveSymbols->setText("Save");
    actions.actionSaveSymbols->setObjectName("actions.actionSaveSymbols");
    actions.actionSaveSymbols->setShortcut(QKeySequence::Save);
    actions.actionSaveSymbols->setEnabled(false);

    actions.actionSaveAsSymbols = new QAction(this);
    actions.actionSaveAsSymbols->setText("Save As...");
    actions.actionSaveAsSymbols->setObjectName("actions.actionSaveAsSymbols");
    actions.actionSaveAsSymbols->setShortcut(QKeySequence::SaveAs);

    actions.actionExit = new QAction(this);
    actions.actionExit->setText("Exit");
    actions.actionExit->setObjectName("actions.actionExit");
    actions.actionExit->setShortcut(QKeySequence::Quit);

    actions.actionAbout = new QAction(this);
    actions.actionAbout->setText("About qCalculator");
    actions.actionAbout->setObjectName("actions.actionAbout");

    actions.actionAboutQt = new QAction(this);
    actions.actionAboutQt->setText("About Qt");
    actions.actionAboutQt->setObjectName("actions.actionAboutQt");

    actions.actionEditSymbols = new QAction(this);
    actions.actionEditSymbols->setText("Edit Symbols");
    actions.actionEditSymbols->setObjectName("actions.actionEditSymbols");
    actions.actionEditSymbols->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));

    actions.actionCompressDirectory = new QAction(this);
    actions.actionCompressDirectory->setText("Compress...");
    actions.actionCompressDirectory->setObjectName("actions.actionCompressDirectory");

    actions.actionExtractArchive = new QAction(this);
    actions.actionExtractArchive->setText("Extract...");
    actions.actionExtractArchive->setObjectName("actions.actionExtractArchive");

    actions.actionCreateAddonBundle = new QAction(this);
    actions.actionCreateAddonBundle->setText("Create Addon Bundle");
    actions.actionCreateAddonBundle->setObjectName("actions.actionCreateAddonBundle");

    actions.actionClearHistory = new QAction(this);
    actions.actionClearHistory->setText("Clear History");
    actions.actionClearHistory->setObjectName("actions.actionClearHistory");
    actions.actionClearHistory->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_X));

    actions.actionAboutPython = new QAction(this);
    actions.actionAboutPython->setText("About Python");
    actions.actionAboutPython->setObjectName("actions.actionAboutPython");

    actions.menuTools->addAction(actions.actionOpenTerminal);
    actions.menuTools->addSeparator();
    actions.menuTools->addAction(actions.actionCompressDirectory);
    actions.menuTools->addAction(actions.actionExtractArchive);
    actions.menuTools->addSeparator();
    actions.menuTools->addAction(actions.actionCreateAddonBundle);

    actions.menuFile->addAction(actions.actionSettings);
    actions.menuFile->addSeparator();
    actions.menuFile->addAction(actions.actionClearHistory);
    actions.menuFile->addSeparator();
    actions.menuFile->addAction(actions.actionExit);

    actions.menuSymbols->addAction(actions.actionEditSymbols);
    actions.menuSymbols->addSeparator();
    actions.menuSymbols->addAction(actions.actionOpenSymbols);
    actions.menuSymbols->addMenu(actions.menuOpenRecent);
    actions.menuSymbols->addAction(actions.actionSaveSymbols);
    actions.menuSymbols->addAction(actions.actionSaveAsSymbols);
    actions.menuSymbols->addSeparator();
    actions.menuSymbols->addAction(actions.actionClearSymbols);

    actions.menuHelp->addAction(actions.actionAbout);
    actions.menuHelp->addAction(actions.actionAboutPython);
    actions.menuHelp->addAction(actions.actionAboutQt);

    menuBar()->addMenu(actions.menuFile);
    menuBar()->addMenu(actions.menuSymbols);
    menuBar()->addMenu(actions.menuTools);
    menuBar()->addMenu(actions.menuHelp);
}

void CalculatorWindow::setupLayout() {
    rootWidget = new QWidget(this);
    rootWidget->setObjectName("widget_root");

    historyWidget = new HistoryWidget(this);
    historyWidget->setObjectName("widget_history");

    input = new QLineEdit(this);
    input->setObjectName("lineEdit_input");
    input->setStyleSheet(
            "QLineEdit{ border-width: 1px; border-style: solid; border-color: palette(base) transparent palette(base) transparent; }");

    auto inputBg = input->palette().color(QPalette::ColorRole::Base);

    inputMessage = new QLabel(this);
    inputMessage->setObjectName("label_input_message");
    inputMessage->setStyleSheet("QLabel { font-weight: bold; color : red; background-color : "
                                + inputBg.name()
                                + "; }");
    inputMessage->setText("F"); // Hack to fix label being slightly smaller before the first non-empty text is set
    inputMessage->setText("");

    input->setContentsMargins(0, 0, 0, 0);
    input->setTextMargins(8, 8, 8, 8);
    inputMessage->setContentsMargins(12, 0, 8, 8);

    auto l = new QVBoxLayout();

    auto *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);

    l->addWidget(historyWidget);
    l->addWidget(line);
    l->addWidget(input);
    l->addWidget(inputMessage);

    line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);

    l->addWidget(line);

    l->setMargin(0);
    l->setSpacing(0);

    rootWidget->setLayout(l);

    auto *footerWidget = new QWidget(this);
    footerWidget->setLayout(new QVBoxLayout);
    footerWidget->setObjectName("footerWidget");
    footerWidget->hide();
    l->addWidget(footerWidget);

    setCentralWidget(rootWidget);
}

void CalculatorWindow::setupDialogs() {
    symbolsDialog = new SymbolsEditorWindow(symbolTable, actions);
    terminalDialog = new PythonConsoleWindow(actions);
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
    connect(terminalDialog,
            SIGNAL(evaluatePython(const std::string &, Interpreter::ParseStyle)),
            this,
            SLOT(onEvaluatePython(const std::string &, Interpreter::ParseStyle)));
}

void CalculatorWindow::cleanupDialogs() {
    symbolsDialog->close();
    terminalDialog->close();
}

void CalculatorWindow::updateSymbolHistoryMenu() {
    auto menu = actions.menuOpenRecent;
    menu->clear();
    for (auto rev = symbolTablePathHistory.rbegin(); rev != symbolTablePathHistory.rend(); rev++) {
        auto path = rev->c_str();
        auto action = menu->addAction(path);
        action->setData(path);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(onActionSymbolTableHistory()));
    }
}

bool CalculatorWindow::loadSymbolTable(const std::string &path) {
    try {
        auto syms = Serializer::deserializeTable(FileOperations::fileReadAll(path));

        std::set<std::string> addons = addonManager->getActiveAddons();
        addonManager->setActiveAddons({});

        symbolTable = syms;
        currentSymbolTablePath = path;

        actions.actionSaveSymbols->setEnabled(true);

        symbolsDialog->setSymbols(symbolTable);

        addonManager->setActiveAddons(addons);

        symbolsDialog->setCurrentSymbolsPath(currentSymbolTablePath);

        return true;
    } catch (const std::exception &e) {
        std::string error = "Failed to load symbols from ";
        error += path;
        error += " Error: ";
        error += e.what();
        QMessageBox::warning(this, "Import failed", error.c_str());

        return false;
    }
}

bool CalculatorWindow::saveSymbolTable(const std::string &path) {
    try {
        FileOperations::fileWriteAll(path, Serializer::serializeTable(symbolTable));

        symbolTablePathHistory.insert(path);
        saveSymbolTablePathHistory();
        updateSymbolHistoryMenu();

        currentSymbolTablePath = path;

        actions.actionSaveSymbols->setEnabled(true);

        symbolsDialog->setCurrentSymbolsPath(currentSymbolTablePath);

        actions.actionSaveSymbols->setEnabled(false);

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
    FileOperations::fileWriteAll(Paths::getHistoryFile(), outputStr);
}

void CalculatorWindow::loadHistory() {
    if (!settings.value(SETTING_SAVE_HISTORY).toInt())
        return;
    if (std::filesystem::exists(Paths::getHistoryFile())) {
        auto str = FileOperations::fileReadAll(Paths::getHistoryFile());

        std::vector<std::string> lines = StringUtil::splitString(str, '\n');
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

void CalculatorWindow::clearResultFromInputText() {
    if (inputTextContainsExpressionResult) {
        inputTextContainsExpressionResult = false;

        auto inputText = input->text().toStdString();

        if (inputText.empty()) {
            input->setText("");
        } else {
            auto size = previousResult.size();

            if (inputText.size() < size) {
                size = inputText.size(); // Delete remaining characters of the result
            }

            // Remove the previous input text contents from the new edited input text
            // This assumes the user can only append to the input line edit when it contains the result
            // When the user moves the text cursor the inputTextContainsExpressionResult flag is reset
            input->setText(inputText.substr(size).c_str());
        }
    }
}

void CalculatorWindow::onInputCursorPositionChanged(int oldPos, int newPos) {
    inputTextContainsExpressionResult = false;
    inputTextAppendedHistoryValue.clear();
    inputTextHistoryIndex = 0;
}

void CalculatorWindow::onEvaluatePython(const std::string &expr, Interpreter::ParseStyle style) {
    if (!Interpreter::isInitialized()) {
        terminalDialog->printError("Python is not initialized.\n");
    } else {
        try {
            Interpreter::runString(expr, style);
        } catch (const std::exception &e) {
            terminalDialog->printError(QString(e.what()) + "\n");
        }
    }
}

void CalculatorWindow::keyPressEvent(QKeyEvent *event) {
    if (input->hasFocus() && !history.empty()) {
        bool increment;
        if (event->key() == Qt::Key_Up) {
            increment = true;
        } else if (event->key() == Qt::Key_Down) {
            increment = false;
        } else {
            QMainWindow::keyPressEvent(event);
            return;
        }

        clearResultFromInputText();

        if (increment)
            inputTextHistoryIndex++;
        else
            inputTextHistoryIndex--;

        if (inputTextHistoryIndex > (int) history.size()) {
            inputTextHistoryIndex = 0;
        } else if (inputTextHistoryIndex < 0) {
            inputTextHistoryIndex = (int) history.size();
        }

        auto index = inputTextHistoryIndex - 1;

        // Remove existing appended value
        if (!inputTextAppendedHistoryValue.empty() && !input->text().isEmpty()) {
            auto cursor = input->cursorPosition() - (int) inputTextAppendedHistoryValue.size();
            input->setText(input->text().remove(cursor,
                                                (int) inputTextAppendedHistoryValue.size()));
            input->setCursorPosition(cursor);
        }

        std::string result;
        if (index < 0) {
            result = "";
        } else {
            result = history.at((int) history.size() - index - 1).second;
        }

        auto inputStr = input->text().toStdString();
        auto cursorPos = input->cursorPosition();
        auto newCursorPos = cursorPos + (int) result.size();
        auto inputBegin = inputStr.substr(0, cursorPos);
        std::string inputEnd;
        if (cursorPos < inputStr.size())
            inputEnd = inputStr.substr(cursorPos);
        input->setText((inputBegin + result + inputEnd).c_str());
        input->setCursorPosition(newCursorPos);

        inputTextAppendedHistoryValue = result;
        inputTextHistoryIndex = index + 1;
    }

    QMainWindow::keyPressEvent(event);
}
