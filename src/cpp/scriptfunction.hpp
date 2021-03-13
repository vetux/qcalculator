#ifndef QT_CALC_SCRIPTFUNCTION
#define QT_CALC_SCRIPTFUNCTION

#include "pythoninterpreter.hpp"

#include <utility>

#include "extern/exprtk.hpp"

/**
 * A exprtk function which executes a single python script.
 *
 * It is a vararg function and all arguments are made available in the argv variable.
 */
template<typename T>
class ScriptFunction : public exprtk::ivararg_function<T> {
public:
    ScriptFunction(PythonInterpreter &interpreter, std::string script)
            : pyInterpreter(interpreter), script(std::move(script)) {}

    inline T operator()(const std::vector<T> &arglist) {
        return pyInterpreter.run(script, arglist);
    }

private:
    PythonInterpreter &pyInterpreter;
    std::string script;
};

#endif //QT_CALC_SCRIPTFUNCTION
