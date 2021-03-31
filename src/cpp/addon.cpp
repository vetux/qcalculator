#include "addon.hpp"

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <stdexcept>

#include "pyutil.hpp"

namespace Addon {
    void load(const std::string &moduleName) {
        PyObject *mod = PyImport_ImportModule(moduleName.c_str());
        if (mod == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *dict = PyModule_GetDict(mod);
        if (dict == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *key = PyUnicode_FromString("load");

        PyObject *function = PyDict_GetItem(dict, key);

        if (function != PyNull) {
            PyObject_CallNoArgs(function);
        }

        Py_DECREF(key);
        Py_DECREF(mod);
    }

    void unload(const std::string &moduleName) {
        PyObject *mod = PyImport_ImportModule(moduleName.c_str());
        if (mod == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *dict = PyModule_GetDict(mod);
        if (dict == PyNull) {
            throw std::runtime_error(PyUtil::getError());
        }

        PyObject *key = PyUnicode_FromString("unload");

        PyObject *function = PyDict_GetItem(dict, key);

        if (function != PyNull) {
            PyObject_CallNoArgs(function);
        }

        Py_DECREF(key);
        Py_DECREF(mod);
    }
}