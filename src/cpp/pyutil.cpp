#include "pyutil.hpp"

#define PY_SSIZE_T_CLEAN

#include <Python.h>

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
