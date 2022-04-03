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

#include "mprealmodule.hpp"

#include "cpython/pythoninclude.hpp"
#include "cpython/pyutil.hpp"

#include "cpython/types/pympreal.hpp"

#include "extern/mpreal.h"

static const char *MODULE_NAME = "_mpreal";

PyObject *mpreal_bits2digits(PyObject *self, PyObject *args);

PyObject *mpreal_digits2bits(PyObject *self, PyObject *args);

static PyMethodDef MethodDef[] = {
        {"bits2digits", mpreal_bits2digits, METH_VARARGS, "."},
        {"digits2bits", mpreal_digits2bits, METH_VARARGS, "."},
        {PyNull, PyNull, 0, PyNull}
};

static PyModuleDef ModuleDef = {
        PyModuleDef_HEAD_INIT,
        MODULE_NAME,
        PyNull,
        -1,
        MethodDef,
        PyNull, PyNull, PyNull, PyNull
};

static PyObject *PyInit() {
    PyObject *m;

    m = PyModule_Create(&ModuleDef);
    if (m == PyNull)
        return PyNull;

    if (!PyMpReal_Initialize(m)) {
        Py_DECREF(m);
        return PyNull;
    }

    return m;
}

PyObject *mpreal_bits2digits(PyObject *self, PyObject *args) {
    int bits;
    if (!PyArg_ParseTuple(args, "i:", &bits)) {
        return PyNull;
    }
    return PyLong_FromSize_t(mpfr::bits2digits(bits));
}

PyObject *mpreal_digits2bits(PyObject *self, PyObject *args) {
    int digits;
    if (!PyArg_ParseTuple(args, "i:", &digits)) {
        return PyNull;
    }
    return PyLong_FromSize_t(mpfr::digits2bits(digits));
}

void MprealModule::initialize() {
    PyImport_AppendInittab(MODULE_NAME, PyInit);
}
