#ifndef QCALC_SCRIPTFUNCTION_HPP
#define QCALC_SCRIPTFUNCTION_HPP

#include "pythoninterpreter.hpp"

#include <utility>

#include "extern/exprtk.hpp"

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
struct ScriptFunction : public exprtk::ifunction<T> {
    using exprtk::ifunction<T>::operator();

    ScriptFunction()
            : exprtk::ifunction<T>(0), interpreter(nullptr) {}

    ScriptFunction(PythonInterpreter &interpreter, std::string script)
            : exprtk::ifunction<T>(0), interpreter(&interpreter), script(std::move(script)) {}

    inline T operator()() {
        assert(interpreter != nullptr);
        return interpreter->run(script, {});
    }

private:
    PythonInterpreter *interpreter;
    std::string script;
};

#endif //QCALC_SCRIPTFUNCTION_HPP
