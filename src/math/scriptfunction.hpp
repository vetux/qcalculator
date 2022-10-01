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

#ifndef QCALC_SCRIPTFUNCTION_HPP
#define QCALC_SCRIPTFUNCTION_HPP

#include <string>

#include "../extern/exprtk.hpp"

#include "scripthandler.hpp"

struct _object;
typedef _object PyObject;

/**
 * The ScriptFunction executes a python function with no arguments.
 * The script must return the result value as a string, float or int.
 */
template<typename T>
struct ScriptFunction : public exprtk::ifunction<T> {
    using exprtk::ifunction<T>::operator();

    ScriptFunction()
            : exprtk::ifunction<T>(0), callback(nullptr) {}

    explicit ScriptFunction(PyObject *callback)
            : exprtk::ifunction<T>(0), callback(callback) {}

    inline T operator()() {
        return ScriptHandler::run(callback, {});
    }

private:
    PyObject *callback;
};

#endif //QCALC_SCRIPTFUNCTION_HPP
