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

#ifndef QCALCULATOR_INTERPRETERHANDLER_HPP
#define QCALCULATOR_INTERPRETERHANDLER_HPP

#include <functional>
#include <string>
#include <set>

#include "calculator/symboltable.hpp"

#include "addon/addonmanager.hpp"

namespace InterpreterHandler {
    void initialize(std::function<void()> onInitialized,
                    std::function<void(const std::string &)> onInitFail,
                    SymbolTable *globalTable,
                    std::function<void()> tableChangeCallback,
                    std::function<void(const std::string &)> stdOutCallback,
                    std::function<void(const std::string &)> stdErrCallback);

    void finalize();

    bool isInitialized();

    bool waitForInitialization(bool interruptable = true);
}

#endif //QCALCULATOR_INTERPRETERHANDLER_HPP
