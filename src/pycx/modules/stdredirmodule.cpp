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

#include <stdexcept>

#include "pycx/modules/stdredirmodule.hpp"

#include "pycx/include.hpp"

static bool redirecting = false;
static std::function<void(const std::string &)> stdOut;
static std::function<void(const std::string &)> stdErr;

static const char *MODULE_NAME = "_stdredir";

PyObject *stdredir_stdout(PyObject *self, PyObject *args);

PyObject *stdredir_stderr(PyObject *self, PyObject *args);

static PyMethodDef MethodDef[] = {
        {"stdout", stdredir_stdout, METH_VARARGS, "."},
        {"stderr", stdredir_stderr, METH_VARARGS, "."},
        {NULL, NULL, 0, NULL}
};

static PyModuleDef ModuleDef = {
        PyModuleDef_HEAD_INIT,
        MODULE_NAME,
        NULL,
        -1,
        MethodDef,
        NULL, NULL, NULL, NULL
};

static PyObject *PyInit() {
    PyObject *m;

    m = PyModule_Create(&ModuleDef);
    if (m == NULL)
        return NULL;

    return m;
}

PyObject *stdredir_stdout(PyObject *self, PyObject *args) {
    char *string;
    if (!PyArg_ParseTuple(args, "s", &string))
        return 0;
    if (stdOut)
        stdOut(string);
    return Py_BuildValue("");
}

PyObject *stdredir_stderr(PyObject *self, PyObject *args) {
    char *string;
    if (!PyArg_ParseTuple(args, "s", &string))
        return 0;
    if (stdErr)
        stdErr(string);
    return Py_BuildValue("");
}

static bool initialized = false;

void StdRedirModule::initialize() {
    if (!initialized)
        PyImport_AppendInittab(MODULE_NAME, PyInit);
    initialized = true;
}

//https://mail.python.org/pipermail/python-list/2002-August/140241.html
static std::string code = R"###(
import _stdredir
import sys
from code import *

class StdOut:
    def write(self, s):
        _stdredir.stdout(s)

class StdErr:
    def write(self, s):
        _stdredir.stderr(s)

def redirect():
    sys.stdout = StdOut()
    sys.stderr = StdErr()
    sys.stdin  = None

redirect()
)###";

void StdRedirModule::startRedirect(std::function<void(const std::string &)> so,
                                   std::function<void(const std::string &)> se) {
    if (redirecting)
        throw std::runtime_error("Already redirecting");

    redirecting = true;

    stdOut = so;
    stdErr = se;

    int r1 = PyRun_SimpleString((code).c_str());

    if (r1 < 0) {
        throw std::runtime_error("Failed to redirect std streams");
    }
}

void StdRedirModule::stopRedirect() {
    if (!redirecting)
        throw std::runtime_error("Not redirecting");

    redirecting = false;

    stdOut = {};
    stdErr = {};

    /*  int r1 = PyRun_SimpleString((code + "undo_redirect()").c_str());
      if (r1 < 0) {
          throw std::runtime_error("Err");
      }*/
}
