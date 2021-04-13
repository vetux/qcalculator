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

#ifndef QCALC_SCRIPTVARARGFUNCTION_HPP
#define QCALC_SCRIPTVARARGFUNCTION_HPP

#include <string>
#include <cassert>

#include "extern/exprtk.hpp"

#include "math/scripthandler.hpp"

struct _object;
typedef _object PyObject;

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
class ScriptVarArgFunction : public exprtk::ivararg_function<T> {
public:
    ScriptVarArgFunction() = default;

    explicit ScriptVarArgFunction(PyObject* callback)
            : callback(callback) {}

    inline T operator()(const std::vector<T> &args) {
        return ScriptHandler::run(callback, args);
    }

private:
    PyObject* callback = nullptr;
};

#endif //QCALC_SCRIPTVARARGFUNCTION_HPP
