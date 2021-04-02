#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <structmember.h>

#include "gui/mainpresenter.hpp"

#include "pymodule/symboltablemodule.hpp"
#include "pymodule/moduletypes.hpp"

#include "pyutil.hpp"

// Try / Catch macros used at the beginning and end of each function block invoked by the python interpreter.
// This is needed because cpython is a c library which does not handle c++ exceptions,
// instead we have to catch all exceptions and give the error messages back to python,
// which in turn causes python to set the error indicator which we then can retrieve again on the invocation site.
#define NATIVE_FUNC_TRY try {
#define NATIVE_FUNC_CATCH } catch (const std::exception &e) { PyErr_SetString(PyExc_RuntimeError, e.what()); return PyNull; }

MainPresenter *presenter = nullptr;

extern "C"
{

static void PySymbolTable_dealloc(PySymbolTable *self) {
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *PySymbolTable_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PySymbolTable *self;
    self = (PySymbolTable *) type->tp_alloc(type, 0);
    return (PyObject *) self;
}

static int PySymbolTable_init(PySymbolTable *self, PyObject *args, PyObject *kwds) {
    if (!PyArg_ParseTuple(args, ":"))
        return -1;
    self->table = {};
    return 0;
}

static PyMemberDef PySymbolTable_members[] = {
        {PyNull}  /* Sentinel */
};

//Workaround for extension types seemingly not being able to be passed as arguments to extension functions.
PyObject *exportToPresenter(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (!PyArg_ParseTuple(args, ":")) {
            return PyNull;
        }

        presenter->setSymbolTable(self->table);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *importFromPresenter(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (!PyArg_ParseTuple(args, ":")) {
            return PyNull;
        }

        self->table = presenter->getSymbolTable();

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *removeSymbol(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        self->table.remove(name);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *getVariableNames(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &t = self->table;
        for (const auto &var : t.getVariables()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *getVariable(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &t = self->table;

        return PyFloat_FromDouble(t.getVariables().at(name));

    NATIVE_FUNC_CATCH
}

PyObject *setVariable(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;
        double value;

        if (PyArg_ParseTuple(args, "sd:", &name, &value) == false) {
            return PyNull;
        }

        self->table.setVariable(name, value);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *getConstantNames(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &t = self->table;
        for (const auto &var : t.getConstants()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *getConstant(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &t = self->table;

        return PyFloat_FromDouble(t.getConstants().at(name));

    NATIVE_FUNC_CATCH
}

PyObject *setConstant(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;
        double value;

        if (PyArg_ParseTuple(args, "sd:", &name, &value) == false) {
            return PyNull;
        }

        self->table.setConstant(name, value);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *getFunctionNames(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &t = self->table;
        for (const auto &var : t.getFunctions()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *getFunctionExpression(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &t = self->table;

        Function ret = t.getFunctions().at(name);
        return PyUnicode_FromString(ret.expression.c_str());

    NATIVE_FUNC_CATCH
}

PyObject *getFunctionArgumentNames(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        const SymbolTable &t = self->table;

        Function ret = t.getFunctions().at(name);

        PyObject *list = PyList_New(0);
        for (auto &arg : ret.argumentNames) {
            PyList_Append(list, PyUnicode_FromString(arg.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *setFunction(PySymbolTable *self, PyObject *args) {
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

        self->table.setFunction(name, f);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *getScriptFunctionNames(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (PyArg_ParseTuple(args, ":") == false) {
            return PyNull;
        }

        PyObject *list = PyList_New(0);

        const SymbolTable &t = self->table;
        for (const auto &var : t.getScripts()) {
            PyList_Append(list, PyUnicode_FromString(var.first.c_str()));
        }

        return list;

    NATIVE_FUNC_CATCH
}

PyObject *getScriptFunctionCallback(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        return self->table.getScripts().at(name).callback;

    NATIVE_FUNC_CATCH
}

PyObject *getScriptFunctionEnableArguments(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *name;

        if (PyArg_ParseTuple(args, "s:", &name) == false) {
            return PyNull;
        }

        return PyBool_FromLong(self->table.getScripts().at(name).enableArguments);

    NATIVE_FUNC_CATCH
}

PyObject *setScriptFunction(PySymbolTable *self, PyObject *args) {
    NATIVE_FUNC_TRY

        const char *functionName;
        PyObject *callback = PyNull;
        unsigned char enableArguments = false;

        if (PyArg_ParseTuple(args, "sOb:", &functionName, &callback, &enableArguments) == false) {
            return PyNull;
        }

        self->table.setScript(functionName, {callback, static_cast<bool>(enableArguments)});

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

static PyMethodDef PySymbolTable_methods[] = {
        {"exportToPresenter",                (PyCFunction) exportToPresenter,                METH_VARARGS, "."},
        {"importFromPresenter",              (PyCFunction) importFromPresenter,              METH_VARARGS, "."},
        {"removeSymbol",                     (PyCFunction) removeSymbol,                     METH_VARARGS, "."},
        {"getVariableNames",                 (PyCFunction) getVariableNames,                 METH_VARARGS, "."},
        {"getVariable",                      (PyCFunction) getVariable,                      METH_VARARGS, "."},
        {"setVariable",                      (PyCFunction) setVariable,                      METH_VARARGS, "."},
        {"getConstantNames",                 (PyCFunction) getConstantNames,                 METH_VARARGS, "."},
        {"getConstant",                      (PyCFunction) getConstant,                      METH_VARARGS, "."},
        {"setConstant",                      (PyCFunction) setConstant,                      METH_VARARGS, "."},
        {"getFunctionNames",                 (PyCFunction) getFunctionNames,                 METH_VARARGS, "."},
        {"getFunctionExpression",            (PyCFunction) getFunctionExpression,            METH_VARARGS, "."},
        {"getFunctionArgumentNames",         (PyCFunction) getFunctionArgumentNames,         METH_VARARGS, "."},
        {"setFunction",                      (PyCFunction) setFunction,                      METH_VARARGS, "."},
        {"getScriptFunctionNames",           (PyCFunction) getScriptFunctionNames,           METH_VARARGS, "."},
        {"getScriptFunctionCallback",        (PyCFunction) getScriptFunctionCallback,        METH_VARARGS, "."},
        {"getScriptFunctionEnableArguments", (PyCFunction) getScriptFunctionEnableArguments, METH_VARARGS, "."},
        {"setScriptFunction",                (PyCFunction) setScriptFunction,                METH_VARARGS, "."},
        {PyNull, PyNull, 0, PyNull}
};

static PyTypeObject PySymbolTableType = {
        PyVarObject_HEAD_INIT(PyNull, 0)
        .tp_name = "qc_native_symtable.SymbolTable",
        .tp_basicsize = sizeof(PySymbolTable),
        .tp_itemsize = 0,
        .tp_dealloc = (destructor) PySymbolTable_dealloc,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_doc = "Symbol Table",
        .tp_methods = PySymbolTable_methods,
        .tp_members = PySymbolTable_members,
        .tp_init = (initproc) PySymbolTable_init,
        .tp_new = PySymbolTable_new,
};

static PyModuleDef PySymbolTableModule = {
        PyModuleDef_HEAD_INIT,
        .m_name = "qc_native_symtable",
        .m_doc = "",
        .m_size = -1,
};

static PyObject *PyInit() {
    PyObject *m;
    if (PyType_Ready(&PySymbolTableType) < 0)
        return PyNull;

    m = PyModule_Create(&PySymbolTableModule);
    if (m == PyNull)
        return PyNull;

    Py_INCREF(&PySymbolTableType);

    if (PyModule_AddObject(m, "SymbolTable", (PyObject *) &PySymbolTableType) < 0) {
        Py_DECREF(&PySymbolTableType);
        Py_DECREF(m);
        return PyNull;
    }

    return m;
}

void SymbolTableModule::initialize(MainPresenter &p) {
    presenter = &p;
    PyImport_AppendInittab("qc_native_symtable", PyInit);
}

}