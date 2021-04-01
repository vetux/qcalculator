#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "nativeinterface.hpp"

#include <map>

#include "pyutil.hpp"

MainView *NativeInterface::view = nullptr;
MainPresenter *NativeInterface::presenter = nullptr;

static PyMethodDef MethodDef[] = {
        {"registerScriptFunction", NativeInterface::registerScriptFunction, METH_VARARGS, "Register a script function to the symbol table."},
        {"unregisterScriptFunction", NativeInterface::unregisterScriptFunction, METH_VARARGS, "Unregister a script function from the symbol table."},
        {PyNull, PyNull, 0, PyNull}
};

static PyModuleDef ModuleDef = {
        PyModuleDef_HEAD_INIT,
        "qci",
        PyNull,
        -1,
        MethodDef,
        PyNull, PyNull, PyNull, PyNull
};

static PyObject *PyInit_qci() {
    return PyModule_Create(&ModuleDef);
}

void NativeInterface::initialize(MainView &v, MainPresenter &p) {
    NativeInterface::view = &v;
    NativeInterface::presenter = &p;
    PyImport_AppendInittab("qci", PyInit_qci);
}

PyObject *NativeInterface::registerScriptFunction(PyObject *self, PyObject *args) {
    SymbolTable table = presenter->getSymbolTable();
    const char *functionName;
    PyObject *callback = PyNull;
    unsigned char enableArguments = false;

    if (PyArg_ParseTuple(args, "sOb:", &functionName, &callback, &enableArguments) == false) {
        return PyNull;
    }

    printf("REGISTER FUNCTION: %s\n", functionName);

    Script script(callback, enableArguments);

    table.setScript(functionName, script);

    presenter->setSymbolTable(table);

    return PyLong_FromLong(0);
}

PyObject *NativeInterface::unregisterScriptFunction(PyObject *self, PyObject *args) {
    SymbolTable table = presenter->getSymbolTable();

    const char *functionName;

    if (PyArg_ParseTuple(args, "s:", &functionName) == false) {
        return PyNull;
    }

    printf("UNREGISTER FUNCTION: %s\n", functionName);

    table.remove(functionName);

    presenter->setSymbolTable(table);

    return PyLong_FromLong(0);
}
