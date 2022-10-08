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

#include <QApplication>
#include <QMessageBox>
#include <QProcess>

#include "windows/calculatorwindow.hpp"

#include "pycx/interpreter.hpp"
#include "pycx/modules/stdredirmodule.hpp"
#include "pycx/modules/exprtkmodule.hpp"

#include "io/paths.hpp"
#include "io/fileoperations.hpp"

#include "settings/settingconstants.hpp"

std::vector<std::string> parseArgs(int argc, char *argv[]) {
    std::vector<std::string> ret;
    ret.reserve(argc);
    for (int i = 0; i < argc; i++) {
        ret.emplace_back(argv[i]);
    }
    return ret;
}

std::wstring getUserPythonPath(Settings &settings) {
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

bool checkPythonInit(std::string &stdErr) {
    QProcess proc;
    proc.start(QApplication::applicationFilePath(), {"--run_python_init_check"}, QIODevice::ReadOnly);
    proc.waitForFinished();
    auto ret = proc.exitCode();
    stdErr = proc.readAllStandardError().toStdString();
    return !ret;
}

std::string configurePython() {
    auto settings = Settings::readSettings();
    auto pythonPath = getUserPythonPath(settings);

    std::string stdErr;

    if (checkPythonInit(stdErr)) {
        if (!pythonPath.empty()) {
            Interpreter::setPath(pythonPath);
        }

        StdRedirModule::initialize();
        ExprtkModule::initialize();
        Interpreter::initialize();
        Interpreter::addModuleDir(Paths::getLibDirectory());
    }

    return stdErr;
}

int runPythonInitCheck() {
    auto settings = Settings::readSettings();
    if (settings.check(SETTING_PYTHON_PATH.key)) {
        auto str = settings.value(SETTING_PYTHON_PATH.key).toString();
        if (!str.empty()) {
            std::wstring wstr;
            for (auto &c: str) {
                wstr += c;
            }
            Interpreter::setPath(wstr);
        }
    }
    Interpreter::initialize();
    Interpreter::finalize();
    return 0;
}

void printUsage() {
    std::cout << "Usage: PROGRAM [ --interpreter, -i ]\n";
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QApplication::setApplicationName("qcalculator");
    QApplication::setApplicationDisplayName("qCalculator");
    QApplication::setApplicationVersion("v0.6.0");

    auto args = parseArgs(argc, argv);

    if (args.size() > 1 && args.at(1) == "--run_python_init_check") {
        return runPythonInitCheck();
    }

    auto err = configurePython();

    if (args.size() > 1) {
        if (args.at(1) == "--interpreter" || args.at(1) == "-i") {
            // Run the application as an interactive python interpreter
            // Does not work on win32 if the application is not using the console subsystem
            if (!Interpreter::isInitialized()) {
                std::cout << "Python is not initialized.\n";
                return 1;
            }
            return Interpreter::runInteractiveLoop();
        } else {
            printUsage();
            return 0;
        }
    } else {
        CalculatorWindow w((err).c_str());
        w.show();
        return QApplication::exec();
    }
}
