/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
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
#include "windows/calculatorwindow.hpp"

#include "pycx/interpreter.hpp"

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

    QApplication::setApplicationName("qCalculator");
    QApplication::setApplicationDisplayName("qCalculator");
    QApplication::setApplicationVersion("v0.6.3");

    auto args = parseArgs(argc, argv);

    if (args.size() > 1 && args.at(1) == "--run_python_init_check") {
        return runPythonInitCheck();
    }

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
        CalculatorWindow w;
        w.show();
        return QApplication::exec();
    }
}
