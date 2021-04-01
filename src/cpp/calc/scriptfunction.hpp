#ifndef QCALC_SCRIPTFUNCTION_HPP
#define QCALC_SCRIPTFUNCTION_HPP

#include <string>

#include "extern/exprtk.hpp"

#include "calc/scripthandler.hpp"

struct _object;
typedef _object PyObject;

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
struct ScriptFunction : public exprtk::ifunction<T> {
    using exprtk::ifunction<T>::operator();

    ScriptFunction()
            : exprtk::ifunction<T>(0), callback(nullptr) {}

    explicit ScriptFunction(PyObject *callback)
            : exprtk::ifunction<T>(0), callback(callback) {}

    inline T operator()() {
        return ScriptHandler::run(callback, {});
    }

private:
    PyObject *callback;
};

#endif //QCALC_SCRIPTFUNCTION_HPP
