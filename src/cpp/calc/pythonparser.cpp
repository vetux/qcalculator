#include "calc/pythonparser.hpp"

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "pyutil.hpp"

std::string GetPythonErrorMessage() {
    PyObject *pType, *pValue, *pTraceback;
    PyErr_Fetch(&pType, &pValue, &pTraceback);

    std::string error = "{ Type: ";
    if (pType != PyNull) {
        PyObject *pTypeStr = PyObject_Str(pType);
        const char *pErrorType = PyUnicode_AsUTF8(pTypeStr);

        error += pErrorType;

        Py_DECREF(pTypeStr);
        Py_DECREF(pType);
    } else {
        error += "NoType";
    }

    error += ", Value: ";
    if (pValue != PyNull) {
        PyObject *pValueStr = PyObject_Str(pValue);
        const char *pErrorValue = PyUnicode_AsUTF8(pValueStr);

        error += pErrorValue;

        Py_DECREF(pValueStr);
        Py_DECREF(pValue);
    } else {
        error += "NoValue";
    }

    error += ", Traceback: ";
    if (pTraceback != PyNull) {
        PyObject *pTracebackStr = PyObject_Str(pTraceback);
        const char *pErrorTraceback = PyUnicode_AsUTF8(pTracebackStr);

        error += pErrorTraceback;

        Py_DECREF(pTracebackStr);
        Py_DECREF(pTraceback);
    } else {
        error += "NoTraceback";
    }

    error += " }";

    return error;
}

double PythonParser::run(const std::string &src, const std::vector<double> &args) {
    PyObject *qcModule = PyImport_ImportModule("qc");

    if (qcModule == PyNull) {
        throw std::runtime_error(GetPythonErrorMessage());
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
        throw std::runtime_error(GetPythonErrorMessage());
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