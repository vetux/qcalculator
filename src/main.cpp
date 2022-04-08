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

#include "gui/mainwindow.hpp"

#include "pycx/interpreter.hpp"

std::vector<std::string> parseArgs(int argc, char *argv[]) {
    std::vector<std::string> ret;
    ret.reserve(argc);
    for (int i = 0; i < argc; i++) {
        ret.emplace_back(argv[i]);
    }
    return ret;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setOrganizationName("Xenotux");
    a.setApplicationName("qcalc");
    a.setApplicationDisplayName("QCalculator");
    a.setApplicationVersion("v0.5.0");

    MainWindow w;

    auto args = parseArgs(argc, argv);

    // Run the application as an interactive python interpreter
    // Does not work on win32 if the application is not using the console subsystem
    if (args.size() > 1) {
        if (args.at(1) == "--interpreter" || args.at(1) == "-i") {
            return Interpreter::runInteractiveLoop();
        }
    }

    w.show();

    return a.exec();
}
