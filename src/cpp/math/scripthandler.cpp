#include "math/scripthandler.hpp"

#include "cpython/pythoninclude.hpp"
#include "cpython/pyutil.hpp"

#include "cpython/types/pympreal.hpp"

ArithmeticType ScriptHandler::run(PyObject *c, const std::vector<ArithmeticType> &a) {
    if (c == PyNull) {
        throw std::runtime_error("Null callback in script handler");
    }

    PyObject *args = PyTuple_New(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        auto &v = a.at(i);
        PyObject *f = PyMpReal_FromMpReal(v);
        PyTuple_SetItem(args, i, f);
    }

    PyObject *pyRet = PyObject_Call(c, args, PyNull);
    Py_DECREF(args);

    if (pyRet == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    mpfr::mpreal ret;

    if (PyMpReal_Check(pyRet)) {
        ret = PyMpReal_AsMpReal(pyRet);
    } else if (PyFloat_Check(pyRet)) {
        ret = PyFloat_AsDouble(pyRet);
    } else if (PyLong_Check(pyRet)) {
        ret = PyLong_AsDouble(pyRet);
    }

    if (PyErr_Occurred() != PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    Py_DECREF(pyRet);

    return ret;
}