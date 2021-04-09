#include "pyutil.hpp"

#include "pythoninclude.hpp"

#include <stdexcept>

void PyUtil::initializePython() {
    Py_Initialize();
}

void PyUtil::finalizePython() {
    Py_Finalize();
}

std::vector<std::string> PyUtil::getModuleDirectories() {
    PyObject *sys_path = PySys_GetObject("path");
    Py_ssize_t size = PyList_Size(sys_path);

    std::vector<std::string> ret;
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *path = PyList_GetItem(sys_path, i);
        ret.emplace_back(PyUnicode_AsUTF8(path));
    }

    return ret;
}

void PyUtil::setModuleDirectories(std::vector<std::string> dirs) {
    PyObject *sys_path = PySys_GetObject("path");
    //PyList_Remove ??!!
    throw std::runtime_error("Not implemented");
}

void PyUtil::addModuleDirectory(const std::string &directory) {
    PyObject *sys_path = PySys_GetObject("path");
    PyList_Append(sys_path, PyUnicode_FromString(directory.c_str()));
}

std::string PyUtil::getError() {
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