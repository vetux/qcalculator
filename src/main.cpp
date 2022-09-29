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
#include <QFileDialog>

#include "gui/calculatorwindow.hpp"

#include "pycx/interpreter.hpp"
#include "pycx/modules/stdredirmodule.hpp"
#include "pycx/modules/exprtkmodule.hpp"

#include "io/paths.hpp"
#include "io/serializer.hpp"
#include "io/fileoperations.hpp"

std::vector<std::string> parseArgs(int argc, char *argv[]) {
    std::vector<std::string> ret;
    ret.reserve(argc);
    for (int i = 0; i < argc; i++) {
        ret.emplace_back(argv[i]);
    }
    return ret;
}

std::wstring getPythonDefaultPath() {
    auto path = Paths::getAppConfigDirectory() + "/pythonDefaultPath.txt";

    QDir("/").mkpath(Paths::getAppConfigDirectory().c_str());

    std::string str;

    if (QFile(path.c_str()).exists()) {
        str = FileOperations::fileReadAllText(path);
    } else {
        if (QMessageBox::question(nullptr,
                                  "Override default python module path",
                                  ("Do you want to configure a python default module path? (Overrides the default os dependent module path, Output is written to " +
                                   path + " )").c_str()) ==
            QMessageBox::StandardButton::Yes) {
            QFileDialog dialog(nullptr);
            dialog.setWindowTitle("Select default python module directories");
            dialog.setFileMode(QFileDialog::Directory);

            std::set<std::string> ret;

            if (dialog.exec()) {
                for (auto &p: dialog.selectedFiles()) {
                    ret.insert(p.toStdString());
                }
                while (QMessageBox::question(nullptr,
                                             "Add Paths",
                                             "Do you want to add more paths?") == QMessageBox::Yes) {
                    if (dialog.exec()) {
                        for (auto &p: dialog.selectedFiles()) {
                            ret.insert(p.toStdString());
                        }
                    }
                }
            } else {
                QMessageBox::information(nullptr,
                                         "Configuration Cancelled",
                                         "Cancelled python default module path configuration");
            }

            for (auto &p: ret) {
                str += p + Interpreter::getDefaultModulePathSeparator();
            }

            if (!str.empty()) {
                str.pop_back();
                FileOperations::fileWriteAllText(path, str);
            }
        } else {
            FileOperations::fileWriteAllText(path, "");
        }
    }

    std::wstring wret;
    for (auto &c: str) {
        wret += c;
    }
    return wret;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QApplication::setOrganizationName("QCalculator");
    QApplication::setApplicationName("qcalc");
    QApplication::setApplicationDisplayName("QCalculator");
    QApplication::setApplicationVersion("v0.6.0");

    // User configurable default python module path for platforms that do have standard paths for python (Eg. Win32)
    // Needs to be separate from other user module paths because the default path cannot be changed after initializing the interpreter.
    auto pyDefaultModuleDir = getPythonDefaultPath();
    if (!pyDefaultModuleDir.empty()) {
        Interpreter::setDefaultModuleDir(pyDefaultModuleDir);
    }

    StdRedirModule::initialize();
    ExprtkModule::initialize();
    Interpreter::initialize();
    Interpreter::addModuleDir(Paths::getAddonDirectory());
    Interpreter::addModuleDir(Paths::getLibDirectory());

    auto args = parseArgs(argc, argv);
    if (args.size() > 1) {
        if (args.at(1) == "--interpreter" || args.at(1) == "-i") {
            // Run the application as an interactive python interpreter
            // Does not work on win32 if the application is not using the console subsystem
            return Interpreter::runInteractiveLoop();
        } else if (args.at(1) == "--terminal" || args.at(1) == "-t") {
            TerminalWindow w;
            w.show();
            return QApplication::exec();
        } else {
            std::cout << "Invalid argument " + std::string(args.at(1));
            return 1;
        }
    } else {
        CalculatorWindow w;
        w.show();
        return QApplication::exec();
    }
}
