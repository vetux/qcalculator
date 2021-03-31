#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "nativeinterface.hpp"

#include <map>

#include "pyutil.hpp"

MainView *NativeInterface::view = nullptr;

static PyMethodDef MethodDef[] = {
        {"showDialog", NativeInterface::showDialog, METH_VARARGS, "Show dialog."},
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

void NativeInterface::initialize(MainView &v) {
    NativeInterface::view = &v;
    PyImport_AppendInittab("qci", PyInit_qci);
}

PyObject *NativeInterface::showDialog(PyObject *self, PyObject *args) {
    view->showQuestionDialog("Question", "Question Text?");
    return PyLong_FromLong(0);
}
