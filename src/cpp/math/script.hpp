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

struct _object;
typedef _object PyObject;

struct Script {
    PyObject *callback = nullptr;
    bool enableArguments = false; //If true the script is wrapped by a vararg function otherwise a function with 0 arguments is used.

    Script() = default;

    Script(PyObject *callback, bool enableArguments)
            : callback(callback), enableArguments(enableArguments) {}

    bool operator==(const Script &other) const {
        return callback == other.callback && enableArguments == other.enableArguments;
    }
};

#endif //QCALC_SCRIPT_HPP
