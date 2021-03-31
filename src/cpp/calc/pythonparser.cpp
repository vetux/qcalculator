#include "calc/pythonparser.hpp"

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "pyutil.hpp"

double PythonParser::run(const std::string &src, const std::vector<double> &args) {
    PyObject *qcModule = PyImport_ImportModule("qcs");

    if (qcModule == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    PyObject *qcDict = PyModule_GetDict(qcModule);

    PyObject *pyArgsList = PyList_New(0);
    for (auto arg : args) {
        PyObject *f = PyFloat_FromDouble(arg);
        PyList_Append(pyArgsList, f);
    }

    PyObject *pyArgsName = PyUnicode_FromString("argv");
    PyDict_SetItem(qcDict, pyArgsName, pyArgsList);

    PyObject *globals = PyDict_New();
    PyObject *locals = PyDict_New();

    PyObject *pyRunStringReturnValue = PyRun_String(src.c_str(), Py_file_input, globals, locals);

    if (pyRunStringReturnValue == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    PyObject *pyOutName = PyUnicode_FromString("outv");
    PyObject *pyOutValue = PyDict_GetItem(qcDict, pyOutName);

    double ret = 0;
    if (pyOutValue != PyNull) {
        ret = PyFloat_AsDouble(pyOutValue);
    }

    PyDict_DelItem(qcDict, pyOutName);
    PyDict_DelItem(qcDict, pyArgsName);

    Py_DECREF(pyOutName);
    Py_DECREF(pyRunStringReturnValue);
    Py_DECREF(locals);
    Py_DECREF(globals);
    Py_DECREF(pyArgsName);
    Py_DECREF(pyArgsList);
    Py_DECREF(qcModule);

    return ret;
}