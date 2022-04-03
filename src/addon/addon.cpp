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

#include "addon.hpp"

#include <utility>
#include <stdexcept>

#include "cpython/pythoninclude.hpp"
#include "cpython/pyutil.hpp"

Addon::Addon(std::string moduleName, std::string displayName, std::string description)
        : loaded(false),
          moduleName(std::move(moduleName)),
          displayName(std::move(displayName)),
          description(std::move(description)) {}

void Addon::callFunctionNoArgs(const std::string &name) {
    moduleLoaded = true;

    PyObject *mod = PyImport_ImportModule(moduleName.c_str());
    if (mod == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    PyObject *dict = PyModule_GetDict(mod);
    if (dict == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }

    PyObject *key = PyUnicode_FromString(name.c_str());

    PyObject *function = PyDict_GetItem(dict, key);

    if (function != PyNull) {
        PyObject *result = PyObject_CallNoArgs(function);
        if (result == PyNull) {
            Py_DECREF(key);
            Py_DECREF(mod);
            throw std::runtime_error(PyUtil::getError());
        }
        Py_DECREF(result);
    } else {
        Py_DECREF(key);
        Py_DECREF(mod);
        throw std::runtime_error("Function not found in addon module: " + name);
    }

    Py_DECREF(key);
    Py_DECREF(mod);
}

void Addon::reload() {
    bool l = loaded;
    if (l)
        unload();

    PyObject *mod = PyImport_ImportModule(moduleName.c_str());
    if (mod == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }
    PyObject *nMod = PyImport_ReloadModule(mod);
    if (nMod == PyNull) {
        throw std::runtime_error(PyUtil::getError());
    }
    Py_DECREF(mod);
    Py_DECREF(nMod);

    if (l)
        load();
}
