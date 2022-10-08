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

#include "math/scripthandler.hpp"

#include "pycx/include.hpp"
#include "pycx/interpreter.hpp"

decimal::Decimal ScriptHandler::run(PyObject *c, const std::vector<decimal::Decimal> &a) {
    if (!Interpreter::isInitialized()){
        throw std::runtime_error("Python is not initialized.");
    }

    if (c == NULL) {
        throw std::runtime_error("Null callback in script handler");
    }

    PyObject *args = PyTuple_New(a.size());
    for (auto i = 0; i < a.size(); i++) {
        auto &v = a.at(i);
        PyObject *f = PyUnicode_FromString(v.format("f").c_str());
        PyTuple_SetItem(args, i, f);
    }

    PyObject *pyRet = PyObject_Call(c, args, NULL);
    Py_DECREF(args);

    if (pyRet == NULL) {
        throw std::runtime_error(Interpreter::getError());
    }

    decimal::Decimal ret;

    if (PyUnicode_Check(pyRet)) {
        auto *str = PyUnicode_AsUTF8(pyRet);
        ret = decimal::Decimal(str);
    } else if (PyFloat_Check(pyRet)) {
        ret = decimal::Decimal(std::to_string(PyFloat_AsDouble(pyRet)));
    } else if (PyLong_Check(pyRet)) {
        ret = decimal::Decimal(std::to_string(PyLong_AsDouble(pyRet)));
    }

    if (PyErr_Occurred() != NULL) {
        throw std::runtime_error(Interpreter::getError());
    }

    Py_DECREF(pyRet);

    return ret;
}