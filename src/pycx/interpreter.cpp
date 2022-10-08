/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "pycx/interpreter.hpp"
#include "pycx/include.hpp"

#include "pycx/modules/stdredirmodule.hpp"
#include "pycx/modules/exprtkmodule.hpp"

static bool pyInitialized = false;

void Interpreter::initialize() {
    if (!pyInitialized)
        Py_Initialize();
    pyInitialized = true;
}

void Interpreter::finalize() {
    if (pyInitialized)
        Py_Finalize();
    pyInitialized = false;
}

bool Interpreter::isInitialized() {
    return pyInitialized;
}

char Interpreter::getPathSeparator() {
// https://docs.python.org/3/c-api/init.html PyGetPath:
// "The returned string consists of a series of directory names separated by a platform dependent delimiter character.
// The delimiter character is ':' on Unix and macOS, ';' on Windows."
#if _WIN32
    return ';';
#elif defined unix || defined __APPLE__
    return ':';
#else
#error "Unsupported target operating system"
#endif
}

void Interpreter::setPath(const std::wstring &path) {
    Py_SetPath(path.c_str());
}

void Interpreter::setPaths(const std::set<std::string> &path) {
    auto sep = getPathSeparator();
    std::string str;
    for (auto &path: path) {
        str += path;
        str += sep;
    }
    if (!str.empty())
        str.pop_back();
    std::wstring wstr;
    for (auto &c: str) {
        wstr += c;
    }
    setPath(wstr);
}

std::wstring Interpreter::getPath() {
    return {Py_GetPath()};
}

void Interpreter::setModuleDirs(const std::vector<std::string> &moduleDirectories) {
    PyObject *list = PyList_New(0);
    for (auto &v: moduleDirectories) {
        PyObject *str = PyUnicode_FromString(v.c_str());
        PyList_Append(list, str);
        Py_DECREF(str);
    }
    PySys_SetObject("path", list);
    Py_DECREF(list);
}

std::vector<std::string> Interpreter::getModuleDirs() {
    PyObject *sys_path = PySys_GetObject("path");
    Py_ssize_t size = PyList_Size(sys_path);

    std::vector<std::string> ret;
    for (Py_ssize_t i = 0; i < size; i++) {
        PyObject *path = PyList_GetItem(sys_path, i);
        ret.emplace_back(PyUnicode_AsUTF8(path));
    }

    return ret;
}

void Interpreter::addModuleDir(const std::string &dir) {
    PyObject *sys_path = PySys_GetObject("path");
    PyList_Append(sys_path, PyUnicode_FromString(dir.c_str()));
}

void Interpreter::removeModuleDir(const std::string &dir) {
    PyObject *sys_path = PySys_GetObject("path");
    auto size = PyList_Size(sys_path);

    PyObject *npath = PyList_New(0);
    for (auto i = 0; i < size; i++) {
        auto *obj = PyList_GetItem(sys_path, i);
        auto str = PyUnicode_AsUTF8(obj);
        if (str != dir) {
            PyList_Append(npath, obj);
        }
    }
    PySys_SetObject("path", npath);
}

int Interpreter::runInteractiveLoop() {
    SymbolTable table;

    auto ret = PyRun_InteractiveLoop(stdin, "stdin");

    return ret;
}

void Interpreter::runString(const std::string &expression,
                            Interpreter::ParseStyle style,
                            const std::string &context) {
    PyObject *n = PyUnicode_FromString(context.c_str());
    PyObject *m = PyImport_GetModule(n);

    if (m == NULL) {
        Py_DECREF(n);
        throw std::runtime_error(getError());
    }

    auto pyStyle = Py_single_input;
    switch (style) {
        case SINGLE_INPUT:
            pyStyle = Py_single_input;
            break;
        case FILE_INPUT:
            pyStyle = Py_file_input;
            break;
        case EVAL_INPUT:
            pyStyle = Py_eval_input;
            break;
        case FUNC_TYPE_INPUT:
            pyStyle = Py_func_type_input;
            break;
    }

    PyObject *g = PyModule_GetDict(m); //Borrowed

    PyObject *r = PyRun_String(expression.c_str(), pyStyle, g, g);

    if (r == NULL) {
        Py_DECREF(m);
        Py_DECREF(n);

        throw std::runtime_error(getError());
    }

    Py_DECREF(r);

    Py_DECREF(m);
    Py_DECREF(n);
}

void Interpreter::callFunctionNoArgs(const std::string &m, const std::string &f) {
    PyObject *mod = PyImport_ImportModule(m.c_str());
    if (mod == NULL) {
        throw std::runtime_error(getError());
    }

    PyObject *dict = PyModule_GetDict(mod);
    if (dict == NULL) {
        throw std::runtime_error(getError());
    }

    PyObject *key = PyUnicode_FromString(f.c_str());

    PyObject *function = PyDict_GetItem(dict, key);

    if (function != NULL) {
        PyObject *result = PyObject_CallNoArgs(function);
        if (result == NULL) {
            Py_DECREF(key);
            Py_DECREF(mod);
            throw std::runtime_error(getError());
        }
        Py_DECREF(result);
    } else {
        Py_DECREF(key);
        Py_DECREF(mod);
        throw std::runtime_error("Function " + f + " not found in module: " + m);
    }

    Py_DECREF(key);
    Py_DECREF(mod);
}

void Interpreter::reloadModule(const std::string &module) {
    PyObject *mod = PyImport_ImportModule(module.c_str());
    if (mod == NULL) {
        throw std::runtime_error(getError());
    }
    PyObject *nMod = PyImport_ReloadModule(mod);
    if (nMod == NULL) {
        throw std::runtime_error(getError());
    }
    Py_DECREF(mod);
    Py_DECREF(nMod);
}

static std::function<void(const std::string &)> _outCallback;
static std::function<void(const std::string &)> _errCallback;

void Interpreter::setStdStreams(const std::function<void(const std::string &)> outCallback,
                                const std::function<void(const std::string &)> errCallback) {
    StdRedirModule::startRedirect(outCallback, errCallback);
}

void Interpreter::clearStdStreams() {
    StdRedirModule::stopRedirect();
}

std::string Interpreter::getError() {
    PyObject *pType, *pValue, *pTraceback;
    PyErr_Fetch(&pType, &pValue, &pTraceback);

    std::string error;
    if (pTraceback != NULL) {
        PyObject *pTracebackStr = PyObject_Str(pTraceback);
        const char *pErrorTraceback = PyUnicode_AsUTF8(pTracebackStr);

        error += pErrorTraceback;

        Py_DECREF(pTracebackStr);
        Py_DECREF(pTraceback);
    } else {
        error += "NoTraceback";
    }

    error += "\n\t";
    if (pType != NULL) {
        PyObject *pTypeStr = PyObject_Str(pType);
        const char *pErrorType = PyUnicode_AsUTF8(pTypeStr);

        error += pErrorType;

        Py_DECREF(pTypeStr);
        Py_DECREF(pType);
    } else {
        error += "NoType";
    }

    error += "\n\t";
    if (pValue != NULL) {
        PyObject *pValueStr = PyObject_Str(pValue);
        const char *pErrorValue = PyUnicode_AsUTF8(pValueStr);

        error += pErrorValue;

        Py_DECREF(pValueStr);
        Py_DECREF(pValue);
    } else {
        error += "NoValue";
    }

    return error;
}

std::string Interpreter::getVersion() {
    return Py_GetVersion();
}

std::string Interpreter::getCopyright() {
    return Py_GetCopyright();
}

std::string Interpreter::getCompiler() {
    return Py_GetCompiler();
}

std::string Interpreter::getBuildInfo() {
    return Py_GetBuildInfo();
}
