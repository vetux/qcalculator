#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "pymodule/pysymboltable.hpp"

#include "pyutil.hpp"

PyObject *PySymbolTable::New(const SymbolTable &table) {
    PyObject *symModule = PyImport_ImportModule("qcalc.sym");
    if (symModule == PyNull) {
        throw std::runtime_error("Failed to import module");
    }

    PyObject *symDict = PyModule_GetDict(symModule);
    PyObject *symClass = PyDict_GetItemString(symDict, "SymbolTable");
    PyObject *funcClass = PyDict_GetItemString(symDict, "Function");
    PyObject *scriptClass = PyDict_GetItemString(symDict, "Script");

    PyObject *symInstance = PyObject_CallNoArgs(symClass);

    PyObject *vars = PyObject_GetAttrString(symInstance, "variables");
    for (auto &var : table.getVariables()) {
        PyDict_SetItemString(vars, var.first.c_str(), PyFloat_FromDouble(var.second));
    }
    Py_DECREF(vars);

    vars = PyObject_GetAttrString(symInstance, "constants");
    for (auto &var : table.getConstants()) {
        PyDict_SetItemString(vars, var.first.c_str(), PyFloat_FromDouble(var.second));
    }
    Py_DECREF(vars);

    vars = PyObject_GetAttrString(symInstance, "functions");
    for (auto &var : table.getFunctions()) {
        PyObject *funcInstance = PyObject_CallNoArgs(funcClass);

        PyObject *argList = PyList_New(0);
        for (auto &argName : var.second.argumentNames) {
            PyList_Append(argList, PyUnicode_FromString(argName.c_str()));
        }

        PyObject_SetAttrString(funcInstance, "expression", PyUnicode_FromString(var.second.expression.c_str()));
        PyObject_SetAttrString(funcInstance, "argument_names", argList);

        PyDict_SetItemString(vars, var.first.c_str(), funcInstance);
    }
    Py_DECREF(vars);

    vars = PyObject_GetAttrString(symInstance, "scripts");
    for (auto &var : table.getScripts()) {
        PyObject *scriptInstance = PyObject_CallNoArgs(scriptClass);

        PyObject_SetAttrString(scriptInstance, "callback", var.second.callback);
        PyObject_SetAttrString(scriptInstance, "enable_arguments", PyBool_FromLong(var.second.enableArguments));

        PyDict_SetItemString(vars, var.first.c_str(), scriptInstance);
    }
    Py_DECREF(vars);

    Py_DECREF(symModule);

    return symInstance;
}

SymbolTable PySymbolTable::Convert(PyObject *o) {
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

        double v;
        if (PyFloat_Check(value)) {
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

        double v;
        if (PyFloat_Check(value)) {
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

    Py_DECREF(attr);

    return ret;
}

SymbolTable PySymbolTable::Cleanup(const SymbolTable &table) {
    SymbolTable ret = table;

    std::vector<std::string> scriptKeys;

    for (auto &script : ret.getScripts()) {
        scriptKeys.emplace_back(script.first);
        Py_DECREF(script.second.callback);
    }

    for (auto &key : scriptKeys) {
        ret.remove(key);
    }

    return ret;
}
