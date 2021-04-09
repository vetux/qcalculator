#include "math/scripthandler.hpp"

#include "cpython/pythoninclude.hpp"
#include "cpython/pyutil.hpp"

double ScriptHandler::run(PyObject *c, const std::vector<double> &a) {
    if (c == PyNull) {
        // Should never happen
        throw std::runtime_error("Null callback in script handler");
    }

    PyObject *args = PyTuple_New(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        auto &v = a.at(i);
        PyObject *f = PyFloat_FromDouble(v);
        PyTuple_SetItem(args, i, f);
    }

    PyObject *pyRet = PyObject_Call(c, args, PyNull);
    Py_DECREF(args);

    if (pyRet == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    double ret = PyFloat_AsDouble(pyRet);

    if (PyErr_Occurred() != PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    Py_DECREF(pyRet);

    return ret;
}