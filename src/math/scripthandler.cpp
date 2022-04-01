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

#include "scripthandler.hpp"

#include "../cpython/pythoninclude.hpp"
#include "../cpython/pyutil.hpp"

#include "../cpython/types/pympreal.hpp"

ArithmeticType ScriptHandler::run(PyObject *c, const std::vector<ArithmeticType> &a) {
    if (c == PyNull) {
        throw std::runtime_error("Null callback in script handler");
    }

    PyObject *args = PyTuple_New(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        auto &v = a.at(i);
        PyObject *f = PyMpReal_FromMpReal(v);
        PyTuple_SetItem(args, i, f);
    }

    PyObject *pyRet = PyObject_Call(c, args, PyNull);
    Py_DECREF(args);

    if (pyRet == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    mpfr::mpreal ret;

    if (PyMpReal_Check(pyRet)) {
        ret = PyMpReal_AsMpReal(pyRet);
    } else if (PyFloat_Check(pyRet)) {
        ret = PyFloat_AsDouble(pyRet);
    } else if (PyLong_Check(pyRet)) {
        ret = PyLong_AsDouble(pyRet);
    }

    if (PyErr_Occurred() != PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    Py_DECREF(pyRet);

    return ret;
}