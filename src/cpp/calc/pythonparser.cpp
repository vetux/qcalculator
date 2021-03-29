#include "calc/pythonparser.hpp"

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#define PyNULL NULL

std::string GetPythonErrorMessage() {
    PyObject *pType, *pValue, *pTraceback;
    PyErr_Fetch(&pType, &pValue, &pTraceback);

    std::string error = "{ Type: ";
    if (pType != PyNULL) {
        PyObject *pTypeStr = PyObject_Str(pType);
        const char *pErrorType = PyUnicode_AsUTF8(pTypeStr);

        error += pErrorType;

        Py_DECREF(pTypeStr);
        Py_DECREF(pType);
    } else {
        error += "NoType";
    }

    error += ", Value: ";
    if (pValue != PyNULL) {
        PyObject *pValueStr = PyObject_Str(pValue);
        const char *pErrorValue = PyUnicode_AsUTF8(pValueStr);

        error += pErrorValue;

        Py_DECREF(pValueStr);
        Py_DECREF(pValue);
    } else {
        error += "NoValue";
    }

    error += ", Traceback: ";
    if (pTraceback != PyNULL) {
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

PythonParser::PythonParser() {
    Py_Initialize();

    PyObject *sys_path = PySys_GetObject("path");
    PyList_Append(sys_path, PyUnicode_FromString("./plugins"));

    qcModule = PyImport_ImportModule("qcalc");
}

PythonParser::~PythonParser() {
    Py_DECREF(qcModule);
    Py_Finalize();
}

double PythonParser::run(const std::string &src, const std::vector<double> &args) {
    PyObject *qcDict = PyModule_GetDict(qcModule);

    if (qcModule == PyNULL) {
        throw std::runtime_error(GetPythonErrorMessage());
    }

    PyObject *pyArgsList = PyList_New(0);
    for (auto arg : args) {
        PyObject *f = PyFloat_FromDouble(arg);
        PyList_Append(pyArgsList, f);
    }

    PyObject *pyArgsName = PyUnicode_FromString("argv");

    PyObject *globals = PyDict_New();
    PyObject *locals = PyDict_New();

    PyDict_SetItem(globals, pyArgsName, pyArgsList);

    PyObject *pyRunStringReturnValue = PyRun_String(src.c_str(), Py_file_input, globals, locals);

    if (pyRunStringReturnValue == PyNULL) {
        throw std::runtime_error(GetPythonErrorMessage());
    }

    PyObject *pyOutName = PyUnicode_FromString("_QCalcOutputValue");

    PyObject *pyOutValue = PyDict_GetItem(qcDict, pyOutName);

    double ret = 0;
    if (pyOutValue != PyNULL) {
        ret = PyFloat_AsDouble(pyOutValue);
    }

    PyDict_DelItem(qcDict, pyOutName);

    Py_DECREF(pyOutName);
    Py_DECREF(pyRunStringReturnValue);
    Py_DECREF(locals);
    Py_DECREF(globals);
    Py_DECREF(pyArgsName);
    Py_DECREF(pyArgsList);

    return ret;
}