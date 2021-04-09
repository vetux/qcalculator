#ifndef QCALC_SCRIPTVARARGFUNCTION_HPP
#define QCALC_SCRIPTVARARGFUNCTION_HPP

#include <string>
#include <cassert>

#include "extern/exprtk.hpp"

#include "math/scripthandler.hpp"

struct _object;
typedef _object PyObject;

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
class ScriptVarArgFunction : public exprtk::ivararg_function<T> {
public:
    ScriptVarArgFunction() = default;

    explicit ScriptVarArgFunction(PyObject* callback)
            : callback(callback) {}

    inline T operator()(const std::vector<T> &args) {
        std::vector<double> pyArgs{args.begin(), args.end()};
        return ScriptHandler::run(callback, pyArgs);
    }

private:
    PyObject* callback = nullptr;
};

#endif //QCALC_SCRIPTVARARGFUNCTION_HPP
