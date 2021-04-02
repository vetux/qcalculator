#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "pymodule/moduletypes.hpp"
#include "pymodule/presentermodule.hpp"

#include "pyutil.hpp"

#define MODULE_NAME "qc_native_presenter" //The name of the exported module accessible by python

// Try / Catch macros used at the beginning and end of each function block invoked by the python interpreter.
// This is needed because cpython is a c library which does not handle c++ exceptions,
// instead we have to catch all exceptions and give the error messages back to python,
// which in turn causes python to set the error indicator which we then can retrieve again on the invocation site.
#define NATIVE_FUNC_TRY try {
#define NATIVE_FUNC_CATCH } catch (const std::exception &e) { PyErr_SetString(PyExc_RuntimeError, e.what()); return PyNull; }

MainPresenter *p = nullptr;

extern "C"
{

PyObject *setSymbolTable(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        PySymbolTable *o;

        if (!PyArg_ParseTuple(args, "O:", &o)) {
            return PyNull;
        }

        p->setSymbolTable(o->table);

        return PyLong_FromLong(0);

    NATIVE_FUNC_CATCH
}

PyObject *getSymbolTable(PyObject *self, PyObject *args) {
    NATIVE_FUNC_TRY

        if (!PyArg_ParseTuple(args, ":"))
            return PyNull;

        //TODO: Find a way to instantiate an extension type in c++.

        throw std::runtime_error("Not Implemented");

    NATIVE_FUNC_CATCH
}

static PyMethodDef MethodDef[] = {
        {"setSymbolTable", setSymbolTable, METH_VARARGS, "."},
        {"getSymbolTable", getSymbolTable, METH_VARARGS, "."},
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
    return PyModule_Create(&ModuleDef);
}

void PresenterModule::initialize(MainPresenter &pres) {
    p = &pres;
    PyImport_AppendInittab(MODULE_NAME, PyInit);
}

}
