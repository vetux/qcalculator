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

#include "symboltableconverter.hpp"

#include "pythoninclude.hpp"
#include "pyutil.hpp"
#include "types/pympreal.hpp"

PyObject *SymbolTableConverter::New(const SymbolTable &table) {
    PyObject *symModule = PyImport_ImportModule("exprtk");
    if (symModule == PyNull) {
        throw std::runtime_error("Failed to import exprtk module, Error: " + PyUtil::getError());
    }

    PyObject *symDict = PyModule_GetDict(symModule);

    PyObject *symClass = PyDict_GetItemString(symDict, "SymbolTable");
    if (symClass == PyNull) {
        throw std::runtime_error("Failed to get SymbolTable class object");
    }

    PyObject *funcClass = PyDict_GetItemString(symDict, "Function");
    if (funcClass == PyNull) {
        throw std::runtime_error("Failed to get Function class object");
    }

    PyObject *scriptClass = PyDict_GetItemString(symDict, "ScriptFunction");
    if (scriptClass == PyNull) {
        throw std::runtime_error("Failed to get ScriptFunction class object");
    }

    PyObject *symInstance = PyObject_CallNoArgs(symClass);

    PyObject *vars = PyObject_GetAttrString(symInstance, "variables");
    for (auto &var : table.getVariables()) {
        PyObject *o = PyMpReal_FromMpReal(var.second);
        PyDict_SetItemString(vars, var.first.c_str(), o);
        Py_DECREF(o);
    }
    Py_DECREF(vars);

    vars = PyObject_GetAttrString(symInstance, "constants");
    for (auto &var : table.getConstants()) {
        PyObject *o = PyMpReal_FromMpReal(var.second);
        PyDict_SetItemString(vars, var.first.c_str(), o);
        Py_DECREF(o);
    }
    Py_DECREF(vars);

    vars = PyObject_GetAttrString(symInstance, "functions");
    for (auto &var : table.getFunctions()) {
        PyObject *funcInstance = PyObject_CallNoArgs(funcClass);

        PyObject *argList = PyList_New(0);
        for (auto &argName : var.second.argumentNames) {
            PyObject *o = PyUnicode_FromString(argName.c_str());
            PyList_Append(argList, o);
            Py_DECREF(o);
        }

        PyObject *o = PyUnicode_FromString(var.second.expression.c_str());

        PyObject_SetAttrString(funcInstance, "expression", o);
        PyObject_SetAttrString(funcInstance, "argument_names", argList);

        PyDict_SetItemString(vars, var.first.c_str(), funcInstance);

        Py_DECREF(o);
        Py_DECREF(argList);
        Py_DECREF(funcInstance);
    }
    Py_DECREF(vars);

    vars = PyObject_GetAttrString(symInstance, "scripts");
    for (auto &var : table.getScripts()) {
        PyObject *scriptInstance = PyObject_CallNoArgs(scriptClass);

        // PyObject_SetAttrString increments reference on passed object, we dont decrement because the returned
        // symbol table holds a new reference to the callback and therefore
        // has to be cleaned up by calling SymbolTableConverter::Cleanup
        PyObject_SetAttrString(scriptInstance, "callback", var.second.callback);

        PyObject *o = PyBool_FromLong(var.second.enableArguments);
        PyObject_SetAttrString(scriptInstance, "enable_arguments", o);

        PyDict_SetItemString(vars, var.first.c_str(), scriptInstance);

        Py_DECREF(o);
        Py_DECREF(scriptInstance);
    }
    Py_DECREF(vars);

    Py_DECREF(symModule);

    return symInstance;
}

SymbolTable SymbolTableConverter::Convert(PyObject *o) {
    SymbolTable ret;

    if (!PyObject_HasAttrString(o, "variables")) {
        throw std::runtime_error("variables attribute must be present");
    }

    PyObject *attr = PyObject_GetAttrString(o, "variables");
    if (!PyDict_Check(attr)) {
        Py_DECREF(attr);
        throw std::runtime_error("variables attribute must be a dictionary");
    }

    PyObject *keys = PyDict_Keys(attr);
    size_t size = PyList_Size(keys);
    for (size_t i = 0; i < size; i++) {
        PyObject *key = PyList_GetItem(keys, i);
        PyObject *value = PyDict_GetItem(attr, key);

        if (!PyUnicode_Check(key)) {
            Py_DECREF(attr);
            throw std::runtime_error("Variable key must be unicode string");
        }

        const char *k = PyUnicode_AsUTF8(key);
        if (k == PyNull) {
            //Should never happen, just in case we will steal the error indicator and throw.
            Py_DECREF(attr);
            throw std::runtime_error(PyUtil::getError());
        }

        mpfr::mpreal v;
        if (PyMpReal_Check(value)) {
            v = PyMpReal_AsMpReal(value);
        } else if (PyFloat_Check(value)) {
            v = PyFloat_AsDouble(value);
        } else if (PyLong_Check(value)) {
            v = PyLong_AsDouble(value);
        } else {
            Py_DECREF(attr);
            throw std::runtime_error("Variable value must be float or long");
        }

        try {
            ret.setVariable(k, v);
        }
        catch (const std::exception &e) {
            Py_DECREF(attr);
            throw e;
        }
    }
    Py_DECREF(keys);
    Py_DECREF(attr);

    if (!PyObject_HasAttrString(o, "constants")) {
        throw std::runtime_error("constants attribute must be present");
    }

    attr = PyObject_GetAttrString(o, "constants");
    if (!PyDict_Check(attr)) {
        Py_DECREF(attr);
        throw std::runtime_error("constants attribute must be a dictionary");
    }

    keys = PyDict_Keys(attr);
    size = PyList_Size(keys);
    for (size_t i = 0; i < size; i++) {
        PyObject *key = PyList_GetItem(keys, i);
        PyObject *value = PyDict_GetItem(attr, key);

        if (!PyUnicode_Check(key)) {
            Py_DECREF(attr);
            throw std::runtime_error("Constant key must be unicode string");
        }

        const char *k = PyUnicode_AsUTF8(key);
        if (k == PyNull) {
            //Should never happen, just in case we will steal the error indicator and throw.
            Py_DECREF(attr);
            throw std::runtime_error(PyUtil::getError());
        }

        mpfr::mpreal v;
        if (PyMpReal_Check(value)) {
            v = PyMpReal_AsMpReal(value);
        } else if (PyFloat_Check(value)) {
            v = PyFloat_AsDouble(value);
        } else if (PyLong_Check(value)) {
            v = PyLong_AsDouble(value);
        } else {
            Py_DECREF(attr);
            throw std::runtime_error("Constant value must be float or long");
        }

        try {
            ret.setConstant(k, v);
        }
        catch (const std::exception &e) {
            Py_DECREF(attr);
            throw e;
        }
    }
    Py_DECREF(keys);
    Py_DECREF(attr);

    if (!PyObject_HasAttrString(o, "functions")) {
        throw std::runtime_error("functions attribute must be present");
    }

    attr = PyObject_GetAttrString(o, "functions");
    if (!PyDict_Check(attr)) {
        Py_DECREF(attr);
        throw std::runtime_error("functions attribute must be a dictionary");
    }

    keys = PyDict_Keys(attr);
    size = PyList_Size(keys);
    for (size_t i = 0; i < size; i++) {
        PyObject *key = PyList_GetItem(keys, i);
        PyObject *value = PyDict_GetItem(attr, key);

        if (!PyUnicode_Check(key)) {
            Py_DECREF(attr);
            throw std::runtime_error("Function key must be unicode string");
        }

        const char *k = PyUnicode_AsUTF8(key);
        if (k == PyNull) {
            //Should never happen, just in case we will steal the error indicator and throw.
            Py_DECREF(attr);
            throw std::runtime_error(PyUtil::getError());
        }

        Function f;
        if (!PyObject_HasAttrString(value, "expression")) {
            Py_DECREF(attr);
            throw std::runtime_error("Function value must have expression attribute");
        } else if (!PyObject_HasAttrString(value, "argument_names")) {
            Py_DECREF(attr);
            throw std::runtime_error("Function value must have argument_names attribute");
        }

        PyObject *funcAttr = PyObject_GetAttrString(value, "expression");
        if (!PyUnicode_Check(funcAttr)) {
            Py_DECREF(funcAttr);
            Py_DECREF(attr);
            throw std::runtime_error("Function expression must be unicode string");
        }

        const char *expr = PyUnicode_AsUTF8(funcAttr);
        if (expr == PyNull) {
            //Should never happen, just in case we will steal the error indicator and throw.
            Py_DECREF(funcAttr);
            Py_DECREF(attr);
            throw std::runtime_error(PyUtil::getError());
        }

        Py_DECREF(funcAttr);

        f.expression = expr;

        funcAttr = PyObject_GetAttrString(value, "argument_names");
        if (PyList_Check(funcAttr)) {
            size_t argSize = PyList_Size(funcAttr);
            for (size_t argi = 0; argi < argSize; argi++) {
                PyObject *pyArgName = PyList_GetItem(funcAttr, argi);
                if (!PyUnicode_Check(pyArgName)) {
                    Py_DECREF(funcAttr);
                    Py_DECREF(attr);
                    throw std::runtime_error("Function argument_name values must be unicode strings");
                }
                const char *argumentName = PyUnicode_AsUTF8(pyArgName);
                if (argumentName == PyNull) {
                    //Should never happen, just in case we will steal the error indicator and throw.
                    Py_DECREF(funcAttr);
                    Py_DECREF(attr);
                    throw std::runtime_error(PyUtil::getError());
                }
                f.argumentNames.emplace_back(argumentName);
            }
        } else if (PySet_Check(funcAttr)) {
            size_t argSize = PySet_Size(funcAttr);
            for (size_t argi = 0; argi < argSize; argi++) {
                PyObject *pyArgName = PySet_Pop(funcAttr);
                if (!PyUnicode_Check(pyArgName)) {
                    Py_DECREF(funcAttr);
                    Py_DECREF(attr);
                    throw std::runtime_error("Function argument_name values must be unicode strings");
                }
                const char *argumentName = PyUnicode_AsUTF8(pyArgName);
                if (argumentName == PyNull) {
                    //Should never happen, just in case we will steal the error indicator and throw.
                    Py_DECREF(funcAttr);
                    Py_DECREF(attr);
                    throw std::runtime_error(PyUtil::getError());
                }
                f.argumentNames.emplace_back(argumentName);
            }
        } else if (PyTuple_Check(funcAttr)) {
            size_t argSize = PyTuple_Size(funcAttr);
            for (size_t argi = 0; argi < argSize; argi++) {
                PyObject *pyArgName = PyTuple_GetItem(funcAttr, argi);
                if (!PyUnicode_Check(pyArgName)) {
                    Py_DECREF(funcAttr);
                    Py_DECREF(attr);
                    throw std::runtime_error("Function argument_name values must be unicode strings");
                }
                const char *argumentName = PyUnicode_AsUTF8(pyArgName);
                if (argumentName == PyNull) {
                    //Should never happen, just in case we will steal the error indicator and throw.
                    Py_DECREF(funcAttr);
                    Py_DECREF(attr);
                    throw std::runtime_error(PyUtil::getError());
                }
                f.argumentNames.emplace_back(argumentName);
            }
        } else {
            Py_DECREF(funcAttr);
            Py_DECREF(attr);
            throw std::runtime_error("Function argument_names must be list, set or tuple");
        }

        Py_DECREF(funcAttr);

        try {
            ret.setFunction(k, f);
        }
        catch (const std::exception &e) {
            Py_DECREF(attr);
            throw e;
        }
    }
    Py_DECREF(keys);
    Py_DECREF(attr);

    if (!PyObject_HasAttrString(o, "scripts")) {
        throw std::runtime_error("scripts attribute must be present");
    }

    attr = PyObject_GetAttrString(o, "scripts");
    if (!PyDict_Check(attr)) {
        Py_DECREF(attr);
        throw std::runtime_error("scripts attribute must be a dictionary");
    }

    keys = PyDict_Keys(attr);
    size = PyList_Size(keys);
    for (size_t i = 0; i < size; i++) {
        PyObject *key = PyList_GetItem(keys, i);
        PyObject *value = PyDict_GetItem(attr, key);

        if (!PyUnicode_Check(key)) {
            Py_DECREF(attr);
            throw std::runtime_error("Script key must be unicode string");
        }

        const char *k = PyUnicode_AsUTF8(key);
        if (k == PyNull) {
            //Should never happen, just in case we will steal the error indicator and throw.
            Py_DECREF(attr);
            throw std::runtime_error(PyUtil::getError());
        }

        Script s;
        if (PyObject_HasAttrString(value, "callback")) {
            // We dont decrement as we want a new pyobject which points to the same callback.
            // When destroying a SymbolTable object with callbacks we have to decrement the reference counts by calling
            // Cleanup().
            s.callback = PyObject_GetAttrString(value, "callback");
        } else {
            Py_DECREF(attr);
            throw std::runtime_error("Script value must have callback attribute");
        }

        if (!PyObject_HasAttrString(value, "enable_arguments")) {
            Py_DECREF(attr);
            throw std::runtime_error("Script value must have enable_arguments attribute");
        }

        PyObject *enableArgs = PyObject_GetAttrString(value, "enable_arguments");
        if (!PyBool_Check(enableArgs)) {
            Py_DECREF(enableArgs);
            Py_DECREF(attr);
            throw std::runtime_error("Function enable_arguments must be bool");
        }

        int eargs = PyObject_IsTrue(enableArgs);
        if (eargs == -1) {
            //Error indicator set? Documentation does not specify so we assume its not.
            eargs = false;
        }

        s.enableArguments = eargs;

        Py_DECREF(enableArgs);

        try {
            ret.setScript(k, s);
        }
        catch (const std::exception &e) {
            Py_DECREF(enableArgs);
            Py_DECREF(attr);
            throw e;
        }
    }
    Py_DECREF(keys);

    Py_DECREF(attr);

    return ret;
}

SymbolTable SymbolTableConverter::Cleanup(const SymbolTable &table) {
    SymbolTable ret = table;

    std::vector<std::string> scriptKeys;

    for (auto &script : ret.getScripts()) {
        scriptKeys.emplace_back(script.first);

        if (script.second.callback == PyNull) {
            // Should never happen
            throw std::runtime_error("Null callback in cleanup");
        }

        Py_DECREF(script.second.callback);
    }

    for (auto &key : scriptKeys) {
        ret.remove(key);
    }

    return ret;
}
