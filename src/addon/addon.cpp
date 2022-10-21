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

#include "addon.hpp"

#include <utility>
#include <stdexcept>

#include "pycx/interpreter.hpp"

Addon::Addon(std::string moduleName, std::string displayName, std::string description)
        : loaded(false),
          moduleName(std::move(moduleName)),
          displayName(std::move(displayName)),
          description(std::move(description)) {}

void Addon::callFunctionNoArgs(const std::string &name) {
    moduleLoaded = true;
    if (Interpreter::isInitialized()) {
        Interpreter::callFunctionNoArgs(moduleName + "." + moduleName, name);
    } else {
        throw std::runtime_error("Python is not initialized (The console contains the error message), ensure that the correct path is configured in the settings.");
    }
}

void Addon::reload() {
    bool l = loaded;
    if (l)
        unload();

    if (Interpreter::isInitialized()) {
        Interpreter::reloadModule(moduleName + "." + moduleName);
    }

    if (l)
        load();
}
