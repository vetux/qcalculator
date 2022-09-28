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

#ifndef QCALC_SCRIPT_HPP
#define QCALC_SCRIPT_HPP

#include <string>
#include <utility>

struct _object;
typedef _object PyObject;

struct Script {
    PyObject *callback = nullptr;
    std::vector<std::string> arguments = {}; //If not empty the script is wrapped by a vararg function otherwise a function with 0 arguments is used.

    Script() = default;

    Script(PyObject *callback, std::vector<std::string> arguments)
            : callback(callback), arguments(std::move(arguments)) {}

    bool operator==(const Script &other) const {
        return callback == other.callback && arguments == other.arguments;
    }
};

#endif //QCALC_SCRIPT_HPP
