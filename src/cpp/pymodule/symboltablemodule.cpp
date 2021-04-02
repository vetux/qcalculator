#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "symboltablemodule.hpp"

#include "pyutil.hpp"

#define MODULE_NAME "qc_native_symtable" //The name of the exported module accessible by python

// Try / Catch macros used at the beginning and end of each function block invoked by the python interpreter.
// This is needed because cpython is a c library which does not handle c++ exceptions,
// instead we have to catch all exceptions and give the error messages back to python,
// which in turn causes python to set the error indicator which we then can retrieve again on the invocation site.
#define NATIVE_FUNC_TRY try {
#define NATIVE_FUNC_CATCH } catch (const std::exception &e) { PyErr_SetString(PyExc_RuntimeError, e.what()); return PyNull; }

MainPresenter *SymbolTableModule::presenter = nullptr;

static PyMethodDef MethodDef[] = {
        {"remove",                           SymbolTableModule::remove,                           METH_VARARGS, "."},
        {"getVariableNames",                 SymbolTableModule::getVariableNames,                 METH_VARARGS, "."},
        {"getVariable",                      SymbolTableModule::getVariable,                      METH_VARARGS, "."},
        {"setVariable",                      SymbolTableModule::setVariable,                      METH_VARARGS, "."},
        {"getConstantNames",                 SymbolTableModule::getConstantNames,                 METH_VARARGS, "."},
        {"getConstant",                      SymbolTableModule::getConstant,                      METH_VARARGS, "."},
        {"setConstant",                      SymbolTableModule::setConstant,                      METH_VARARGS, "."},
        {"getFunctionNames",                 SymbolTableModule::getFunctionNames,                 METH_VARARGS, "."},
        {"getFunctionExpression",            SymbolTableModule::getFunctionExpression,            METH_VARARGS, "."},
        {"getFunctionArgumentNames",         SymbolTableModule::getFunctionArgumentNames,         METH_VARARGS, "."},
        {"setFunction",                      SymbolTableModule::setFunction,                      METH_VARARGS, "."},
        {"getScriptFunctionNames",           SymbolTableModule::getScriptFunctionNames,           METH_VARARGS, "."},
        {"getScriptFunctionCallback",        SymbolTableModule::getScriptFunctionCallback,        METH_VARARGS, "."},
        {"getScriptFunctionEnableArguments", SymbolTableModule::getScriptFunctionEnableArguments, METH_VARARGS, "."},
        {"setScriptFunction",                SymbolTableModule::setScriptFunction,                METH_VARARGS, "."},
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

static PyObject *PyInit_qci() {
    return PyModule_Create(&ModuleDef);
}

void SymbolTableModule::initialize(MainPresenter &p) {
    SymbolTableModule::presenter = &p;
    PyImport_AppendInittab(MODULE_NAME, PyInit_qci);
}

PyObject *SymbolTableModule::remove(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        SymbolTable table = presenter->getSymbolTable();
        table.remove(name);

        presenter->setSymbolTable(table);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getVariableNames(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &table = presenter->getSymbolTable();
        for (const auto &var : table.getVariables()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getVariable(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &table = presenter->getSymbolTable();

        double ret = table.getVariables().at(name);
        return PyFloat_FromDouble(ret);

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::setVariable(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;
        double value;

        if (PyArg_ParseTuple(args, "sd:", &name, &value) == false) {
            return PyNull;
        }

        SymbolTable table = presenter->getSymbolTable();

        table.setVariable(name, value);

        presenter->setSymbolTable(table);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getConstantNames(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &table = presenter->getSymbolTable();
        for (const auto &var : table.getConstants()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getConstant(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &table = presenter->getSymbolTable();

        double ret = table.getConstants().at(name);

        return PyFloat_FromDouble(ret);

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::setConstant(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;
        double value;

        if (PyArg_ParseTuple(args, "sd:", &name, &value) == false) {
            return PyNull;
        }

        SymbolTable table = presenter->getSymbolTable();

        table.setConstant(name, value);

        presenter->setSymbolTable(table);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getFunctionNames(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &table = presenter->getSymbolTable();
        for (const auto &var : table.getFunctions()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getFunctionExpression(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &table = presenter->getSymbolTable();

        Function ret = table.getFunctions().at(name);
        return PyUnicode_FromString(ret.expression.c_str());

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getFunctionArgumentNames(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &table = presenter->getSymbolTable();

        Function ret = table.getFunctions().at(name);

        PyObject *list = PyList_New(0);
        for (auto &arg : ret.argumentNames) {
            PyList_Append(list, PyUnicode_FromString(arg.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::setFunction(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;
        const char *expression;
        PyObject *argumentNames;

        if (PyArg_ParseTuple(args, "ssO:", &name, &expression, &argumentNames) == false) {
            return PyNull;
        }

        Function f;
        f.expression = expression;

        if (PyList_Check(argumentNames)) {
            for (size_t i = 0; i < PyList_Size(argumentNames); i++) {
                f.argumentNames.emplace_back(PyUnicode_AsUTF8(PyList_GetItem(argumentNames, i)));
            }
        } else if (PySet_Check(argumentNames)) {
            size_t numberOfItems = PySet_Size(argumentNames);
            for (size_t i = 0; i < numberOfItems; i++) {
                PyObject *o = PySet_Pop(argumentNames);
                f.argumentNames.emplace_back(PyUnicode_AsUTF8(o));
                Py_DECREF(o);
            }
        } else if (PyTuple_Check(argumentNames)) {
            for (size_t i = 0; i < PyTuple_Size(argumentNames); i++) {
                f.argumentNames.emplace_back(PyUnicode_AsUTF8(PyTuple_GetItem(argumentNames, i)));
            }
        } else {
            Py_DECREF(argumentNames);
            PyErr_SetString(PyExc_RuntimeError, "Argument names must be list, set or tuple.");
            return PyNull;
        }

        Py_DECREF(argumentNames);

        SymbolTable table = presenter->getSymbolTable();

        table.setFunction(name, f);

        presenter->setSymbolTable(table);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getScriptFunctionNames(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &table = presenter->getSymbolTable();
        for (const auto &var : table.getScripts()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getScriptFunctionCallback(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &table = presenter->getSymbolTable();

        try {
            Script ret = table.getScripts().at(name);

            return ret.callback;
        } catch (const std::runtime_error &e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
            return PyNull;
        }

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::getScriptFunctionEnableArguments(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &table = presenter->getSymbolTable();

        Script ret = table.getScripts().at(name);

        return PyBool_FromLong(ret.enableArguments);

    NATIVE_FUNC_CATCH
}

PyObject *SymbolTableModule::setScriptFunction(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *functionName;
        PyObject *callback = PyNull;
        unsigned char enableArguments = false;

        if (PyArg_ParseTuple(args, "sOb:", &functionName, &callback, &enableArguments) == false) {
            return PyNull;
        }

        Script script(callback, enableArguments);

        SymbolTable table = presenter->getSymbolTable();

        table.setScript(functionName, script);

        presenter->setSymbolTable(table);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}