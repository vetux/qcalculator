#ifndef QCALC_SCRIPT_HPP
#define QCALC_SCRIPT_HPP

#include <string>

struct _object;
typedef _object PyObject;

struct Script {
    PyObject *callback = nullptr;
    bool enableArguments = false; //If true the script is wrapped by a vararg function otherwise a function with 0 arguments is used.

    Script() = default;

    Script(PyObject *callback, bool enableArguments)
            : callback(callback), enableArguments(enableArguments) {}

    bool operator==(const Script &other) const {
        return callback == other.callback && enableArguments == other.enableArguments;
    }
};

#endif //QCALC_SCRIPT_HPP
