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
#include <QProcess>

#include "gui/calculatorwindow.hpp"

#include "pycx/interpreter.hpp"
#include "pycx/modules/stdredirmodule.hpp"
#include "pycx/modules/exprtkmodule.hpp"

#include "io/paths.hpp"
#include "io/serializer.hpp"
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
    auto path = Paths::getSettingsFile();

    std::string str;

    if (settings.check(SETTING_PYTHON_PATH.key)) {
        str = settings.value(SETTING_PYTHON_PATH.key).toString();
    } else {
        if (QMessageBox::question(nullptr,
                                  "Configure python default path",
                                  "Do you want to override the python default path?")
            == QMessageBox::StandardButton::Yes) {
            QFileDialog dialog(nullptr);
            dialog.setWindowTitle("Select directories to add");
            dialog.setFileMode(QFileDialog::Directory);

            std::set<std::string> ret;

            if (dialog.exec()) {
                for (auto &p: dialog.selectedFiles()) {
                    ret.insert(p.toStdString());
                }
                while (QMessageBox::question(nullptr,
                                             "Add directories",
                                             "Do you want to add more directories to the path?") == QMessageBox::Yes) {
                    if (dialog.exec()) {
                        for (auto &p: dialog.selectedFiles()) {
                            ret.insert(p.toStdString());
                        }
                    } else {
                        QMessageBox::information(nullptr,
                                                 "Path configuration cancelled",
                                                 "Cancelled python default path configuration");
                        return {};
                    }
                }
            } else {
                QMessageBox::information(nullptr,
                                         "Path configuration cancelled",
                                         "Cancelled python default path configuration");
                return {};
            }

            for (auto &p: ret) {
                str += p + Interpreter::getPathSeparator();
            }

            if (!str.empty()) {
                str.pop_back();
                settings.update(SETTING_PYTHON_PATH.key, str);
            }
        } else {
            settings.update(SETTING_PYTHON_PATH.key, std::string());
        }
    }

    Settings::saveSettings(settings);

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

void configurePythonPath() {
    auto settings = Settings::readSettings();
    auto pythonPath = getUserPythonPath(settings);

    std::string stdErr;
    while (!checkPythonInit(stdErr)) {
        if (QMessageBox::question(nullptr, "Python initialization failed",
                                  ("Python failed to initialize do you want to reconfigure the python path?\n\n" +
                                   stdErr).c_str())
            == QMessageBox::Yes) {
            settings.clear(SETTING_PYTHON_PATH);
            pythonPath = getUserPythonPath(settings);
        } else {
            break;
        }
    }
    if (!pythonPath.empty()) {
        Interpreter::setPath(pythonPath);
    }
}

int runPythonInitCheck() {
    auto settings = Settings::readSettings();
    if (settings.check(SETTING_PYTHON_PATH.key)) {
        auto str = settings.value(SETTING_PYTHON_PATH.key).toString();
        if (!str.empty()){
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

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QApplication::setApplicationName("qcalculator");
    QApplication::setApplicationDisplayName("QCalculator");
    QApplication::setApplicationVersion("v0.6.0");

    auto args = parseArgs(argc, argv);

    if (args.size() > 1 && args.at(1) == "--run_python_init_check") {
        return runPythonInitCheck();
    }

    configurePythonPath();

    StdRedirModule::initialize();
    ExprtkModule::initialize();
    Interpreter::initialize();
    Interpreter::addModuleDir(Paths::getLibDirectory());

    if (args.size() > 1) {
        if (args.at(1) == "--interpreter" || args.at(1) == "-i") {
            // Run the application as an interactive python interpreter
            // Does not work on win32 if the application is not using the console subsystem
            return Interpreter::runInteractiveLoop();
        } else if (args.at(1) == "--terminal" || args.at(1) == "-t") {
            TerminalWindow w;
            w.show();
            return QApplication::exec();
        }
    } else {
        CalculatorWindow w;
        w.show();
        return QApplication::exec();
    }
}
