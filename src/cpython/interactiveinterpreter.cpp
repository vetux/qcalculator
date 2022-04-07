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

#include "interactiveinterpreter.hpp"

#include <cstdio>

#include "cpython/pythoninclude.hpp"
#include "cpython/modules/exprtkmodule.hpp"
#include "cpython/pyutil.hpp"
#include "io/paths.hpp"

#include "cpython/modules/stdredirmodule.hpp"

void InteractiveInterpreter::initialize() {
    StdRedirModule::initialize();
}

int InteractiveInterpreter::run() {
    SymbolTable table;

    ExprtkModule::initialize(table, []() {});

    PyUtil::initializePython();

    PyUtil::addModuleDirectory(Paths::getSystemDirectory());
    PyUtil::addModuleDirectory(Paths::getAddonDirectory());

    auto ret = PyRun_InteractiveLoop(stdin, "stdin");

    PyUtil::finalizePython();

    return ret;
}

std::string InteractiveInterpreter::runString(const std::string &expression,
                                              ParseStyle style,
                                              const std::string &context) {
    PyObject *n = PyUnicode_FromString(context.c_str());
    PyObject *m = PyImport_GetModule(n);

    if (m == PyNull) {
        Py_DECREF(n);
        throw std::runtime_error(PyUtil::getError());
    }

    auto pyStyle = Py_single_input;
    switch (style) {
        case SINGLE_INPUT:
            pyStyle = Py_single_input;
            break;
        case FILE_INPUT:
            pyStyle = Py_file_input;
            break;
        case EVAL_INPUT:
            pyStyle = Py_eval_input;
            break;
        case FUNC_TYPE_INPUT:
            pyStyle = Py_func_type_input;
            break;
    }
    std::string output;

    StdRedirModule::startRedirect([&output](const std::string &str) {
                                      output += str;
                                  },
                                  [&output](const std::string &str) {
                                      output += str;
                                  });

    PyObject *g = PyModule_GetDict(m); //Borrowed

    PyObject *r = PyRun_String(expression.c_str(), pyStyle, g, g);

    StdRedirModule::stopRedirect();

    if (r == PyNull) {
        Py_DECREF(m);
        Py_DECREF(n);

        throw std::runtime_error(PyUtil::getError());
    }

    Py_DECREF(r);

    Py_DECREF(m);
    Py_DECREF(n);

    return output;
}
