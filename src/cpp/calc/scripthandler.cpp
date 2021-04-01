#include "calc/scripthandler.hpp"

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "pyutil.hpp"

double ScriptHandler::run(PyObject *c, const std::vector<double> &a) {
    PyObject *args = PyTuple_New(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        auto &v = a.at(i);
        PyObject *f = PyFloat_FromDouble(v);
        PyTuple_SetItem(args, i, f);
    }

    PyObject *kwargs = PyNull;

    PyObject *ret = PyObject_Call(c, args, kwargs);
    Py_DECREF(args);

    if (ret == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    return PyFloat_AsDouble(ret);
}