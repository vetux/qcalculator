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

#include "interpreterhandler.hpp"

#include <thread>
#include <condition_variable>
#include <mutex>

#include <QProcess>
#include <QApplication>
#include <QMessageBox>
#include <QPushButton>
#include <QFile>

#include <utility>


#include "python/modules/stdredirmodule.hpp"
#include "python/modules/exprtkmodule.hpp"
#include "python/interpreter.hpp"

#include "io/paths.hpp"

#include "settings/settings.hpp"
#include "settings/settingconstants.hpp"

#include "io/serializer.hpp"

static bool threadRunning = false;

static std::thread thread;

static bool quitThread = false;
static std::mutex quitThreadMutex;
static std::condition_variable quitThreadCondition;

static bool initFinish = false;

static std::function<void()> onInitialized;
static std::function<void(const std::string &)> onInitFail;

static SymbolTable *symbolTable;
static std::function<void()> tableChangeCallback;

static std::function<void(const std::string &)> outCallback;
static std::function<void(const std::string &)> errCallback;

static std::wstring getUserPythonPath(Settings &settings) {
    std::string str;

    if (settings.check(SETTING_PYTHON_PATH.key)) {
        str = settings.value(SETTING_PYTHON_PATH.key).toString();
    }

    if (str.empty()) {
        return {};
    } else {
        std::wstring wret;
        for (auto &c: str) {
            wret += c;
        }
        return wret;
    }
}

static bool checkPythonInit(std::string &stdErr) {
    QProcess proc;
    proc.start(QApplication::applicationFilePath().toStdString().c_str(), {"--run_python_init_check"}, QIODevice::ReadOnly);
    proc.waitForFinished();
    auto ret = proc.exitCode();
    stdErr = proc.readAllStandardError().toStdString();
    return !ret;
}

static void initializePython(const std::wstring &pythonPath) {
    if (!pythonPath.empty()) {
        Interpreter::setPath(pythonPath);
    }

    StdRedirModule::initialize();
    ExprtkModule::initialize();
    Interpreter::initialize();
    Interpreter::addModuleDir(Paths::getLibDirectory());
    Interpreter::addModuleDir(Paths::getAddonDirectory());
}

static void threadLoop() {
    std::unique_lock lk(quitThreadMutex);

    auto settings = Settings::readSettings();
    auto pythonPath = getUserPythonPath(settings);

    std::string stdErr;

    auto pythonOk = checkPythonInit(stdErr);

    if (pythonOk) {
        initializePython(pythonPath);

        Interpreter::setStdStreams(outCallback, errCallback);

        for (auto &path: settings.value(SETTING_PYTHON_MODULE_PATHS).toStringList()) {
            Interpreter::addModuleDir(path);
        }

        ExprtkModule::setGlobalTable(*symbolTable, tableChangeCallback);

        Interpreter::saveThreadState();
    }

    initFinish = true;

    if (pythonOk) {
        onInitialized();
    } else {
        onInitFail(stdErr);
    }

    // Keep thread running because python installs signal handlers for the thread that calls PyInitialize
    quitThreadCondition.wait(lk, [] { return quitThread; });
}

void InterpreterHandler::initialize(std::function<void()> onInitializedCallback,
                                    std::function<void(const std::string &)> onInitFailCallback,
                                    SymbolTable *globalTable,
                                    std::function<void()> tableChangeCallbackArg,
                                    std::function<void(const std::string &)> stdOutCallback,
                                    std::function<void(const std::string &)> stdErrCallback) {
    if (threadRunning) {
        throw std::runtime_error("Interpreter already initializing.");
    }
    threadRunning = true;

    onInitialized = std::move(onInitializedCallback);
    onInitFail = std::move(onInitFailCallback);

    symbolTable = globalTable;
    tableChangeCallback = std::move(tableChangeCallbackArg);

    outCallback = std::move(stdOutCallback);
    errCallback = std::move(stdErrCallback);

    thread = std::thread(threadLoop);
}

void InterpreterHandler::finalize() {
    {
        std::lock_guard lk(quitThreadMutex);
        quitThread = true;
    }
    quitThreadCondition.notify_all();
    thread.join();
}

bool InterpreterHandler::isInitialized() {
    return initFinish;
}

bool InterpreterHandler::waitForInitialization(bool interruptable) {
    if (!initFinish) {
        bool cancel = false;

        auto *button = new QPushButton();
        button->setText("Cancel");
        button->update();

        QPushButton::connect(button, &QPushButton::clicked, [&cancel](bool b) {
            cancel = true;
        });

        auto *messageBox = new QMessageBox;
        messageBox->setWindowTitle("Python is initializing");
        messageBox->setText("Waiting for Python initialization to complete...");
        messageBox->setStandardButtons(QMessageBox::NoButton);
        if (interruptable) {
            messageBox->addButton(button, QMessageBox::ButtonRole::RejectRole);
        }
        messageBox->setWindowFlag(Qt::WindowType::WindowStaysOnTopHint, true);
        messageBox->show();
        messageBox->update();

        // Busy wait is required to update the message box event loop to allow the user to cancel
        while (true) {
            if (initFinish || cancel) {
                break;
            }
            QCoreApplication::processEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(125));
        }

        delete messageBox;
    }

    return initFinish;
}
