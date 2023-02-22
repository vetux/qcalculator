/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "exprtkmodule.hpp"

#include <utility>

#include "pycx/pythoninclude.hpp"
#include "pycx/symboltableutil.hpp"

#include "math/expressionparser.hpp"

#include "modulecommon.hpp"

#define MODULE_NAME "_exprtk"

static SymbolTable *symbolTable = nullptr;
static std::function<void()> symbolTableCallback;

PyObject *evaluate(PyObject *self, PyObject *args) {
    MODULE_FUNC_TRY

        PyObject *pyExpression;
        PyObject *pySymTable;

        if (!PyArg_ParseTuple(args, "OO:", &pyExpression, &pySymTable)) {
            return NULL;
        }

        const char *expression = PyUnicode_AsUTF8(pyExpression);
        if (expression == NULL) {
            return NULL;
        }

        SymbolTable symTable = SymbolTableUtil::Convert(pySymTable);

        decimal::Decimal value = ExpressionParser::evaluate(expression, symTable);

        PyObject *ret = PyTuple_New(2);

        PyTuple_SetItem(ret, 0, PyFloat_FromDouble(std::stod(value.format("f"))));
        PyTuple_SetItem(ret, 1, SymbolTableUtil::New(symTable));

        SymbolTableUtil::Cleanup(symTable);

        return ret;

    MODULE_FUNC_CATCH
}

PyObject *get_global_symtable(PyObject *self, PyObject *args) {
    MODULE_FUNC_TRY

        if (symbolTable == nullptr)
            return nullptr;
        else
            return SymbolTableUtil::New(*symbolTable);

    MODULE_FUNC_CATCH
}

PyObject *set_global_symtable(PyObject *self, PyObject *args) {
    MODULE_FUNC_TRY

        PyObject *pysym = NULL;
        if (!PyArg_ParseTuple(args, "O:", &pysym)) {
            return NULL;
        }
        SymbolTable &t = *symbolTable;
        auto table = SymbolTableUtil::Convert(pysym);

        if (symbolTableCallback
            && !table.equalsExcludeScripts((t))) {
            symbolTableCallback();
        }

        t = table;

        return PyLong_FromLong(0);

    MODULE_FUNC_CATCH
}

static PyMethodDef MethodDef[] = {
        {"evaluate",            evaluate,            METH_VARARGS, "."},
        {"get_global_symtable", get_global_symtable, METH_NOARGS,  "."},
        {"set_global_symtable", set_global_symtable, METH_VARARGS, "."},
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

void ExprtkModule::initialize() {
    PyImport_AppendInittab(MODULE_NAME, PyInit);
}

void ExprtkModule::setGlobalTable(SymbolTable &globalTable, std::function<void()> tableChangeCallback) {
    symbolTable = &globalTable;
    symbolTableCallback = std::move(tableChangeCallback);
}
