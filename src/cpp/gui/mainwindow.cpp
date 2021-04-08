#include "gui/mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDir>
#include <QMessageBox>

#include "pyutil.hpp"
#include "addonmanager.hpp"
#include "addonhelper.hpp"
#include "paths.hpp"
#include "serializer.hpp"
#include "io.hpp"
#include "numberformat.hpp"

#include "calc/expressionparser.hpp"

#include "gui/settingsdialog.hpp"
#include "gui/widgets/historywidget.hpp"
#include "gui/widgets/symbolseditor.hpp"

#include "pymodule/exprtkmodule.hpp"

#define ADDONS_FILE "/addons.json"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow()) {
    ui->setupUi(this);

    history = new HistoryWidget(this);
    history->setObjectName("widget_history");

    ui->tab_history->layout()->addWidget(history);

    symbolsEditor = new SymbolsEditor(this);
    symbolsEditor->setObjectName("widget_symtable_editor");

    connect(symbolsEditor, SIGNAL(onSymbolsChanged(const SymbolTable &)), this, SLOT(onSymbolTableChanged(const SymbolTable &)));

    ui->tab_symbols->layout()->addWidget(symbolsEditor);

    connect(ui->actionSettings, SIGNAL(triggered(bool)), this, SLOT(onActionSettings()));
    connect(ui->actionExit, SIGNAL(triggered(bool)), this, SLOT(onActionExit()));
    connect(ui->actionAbout, SIGNAL(triggered(bool)), this, SLOT(onActionAbout()));

    connect(ui->lineEdit_input, SIGNAL(returnPressed()), this, SLOT(onInputReturnPressed()));

    connect(this,
            SIGNAL(signalInputTextChange(const QString &)),
            ui->lineEdit_input,
            SLOT(setText(const QString &)));

    connect(this,
            SIGNAL(signalExpressionEvaluated(const QString &, const QString &)),
            history,
            SLOT(addContent(const QString &, const QString &)));

    ExprtkModule::initialize();

    PyUtil::initializePython();
    PyUtil::addModuleDirectory(Paths::getSystemDirectory());
    PyUtil::addModuleDirectory(Paths::getAddonDirectory());

    std::string enabledAddonsFilePath = Paths::getAppDataDirectory().append(ADDONS_FILE);

    std::set<std::string> enabledAddons;
    if (QFile(enabledAddonsFilePath.c_str()).exists()) {
        try {
            enabledAddons = Serializer::deserializeSet(IO::fileReadAllText(enabledAddonsFilePath));
        }
        catch (const std::runtime_error &e) {
            QMessageBox::warning(this, "Failed to read enabled addons file", e.what());
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
    AddonManager::setActiveAddons({}, *this);
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

        symbolsEditor->setSymbols(symbolTable);

        emit signalExpressionEvaluated(inputText.c_str(), NumberFormat::toDecimal(value).c_str());

        inputText = NumberFormat::toDecimal(value);

        emit signalInputTextChange(inputText.c_str());
    }
    catch (const std::runtime_error &e) {
        QMessageBox::warning(this, "Failed to evaluate expression", e.what());
    }
}

void MainWindow::onSymbolTableChanged(const SymbolTable &symbolTableArg) {
    this->symbolTable = symbolTableArg;
    symbolsEditor->setSymbols(symbolTable);
}

void MainWindow::onActionSettings() {
    SettingsDialog dialog;
    dialog.setEnabledAddons(AddonManager::getActiveAddons());
    dialog.show();
    if (dialog.exec() == QDialog::Accepted) {
        std::set<std::string> addons = dialog.getEnabledAddons();

        AddonManager::setActiveAddons(addons, *this);

        try {
            std::string serializedAddons = Serializer::serializeSet(addons);

            try {
                std::string dataDir = Paths::getAppDataDirectory();

                if (!QDir(dataDir.c_str()).exists())
                    QDir().mkpath(dataDir.c_str());

                IO::fileWriteAllText(dataDir.append(ADDONS_FILE), serializedAddons);
            }
            catch (const std::runtime_error &e) {
                QMessageBox::warning(this, "Failed to write enabled addons file", e.what());
            }
        }
        catch (const std::runtime_error &e) {
            QMessageBox::warning(this, "Failed to serialize enabled addons", e.what());
        }
    }
}

void MainWindow::onActionExit() {
    AddonManager::setActiveAddons({}, *this);
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
    https://github.com/ArashPartow/exprtk
    https://github.com/python/cpython
    https://github.com/qt
)LLL"));
}

void MainWindow::onActionImportSymbolTable() {
}

void MainWindow::onActionExportSymbolTable() {
}
