#ifndef QCALC_SCRIPTFUNCTION_HPP
#define QCALC_SCRIPTFUNCTION_HPP

#include <string>

#include "extern/exprtk.hpp"

#include "calc/pythonparser.hpp"

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
struct ScriptFunction : public exprtk::ifunction<T> {
    using exprtk::ifunction<T>::operator();

    ScriptFunction()
            : exprtk::ifunction<T>(0) {}

    explicit ScriptFunction(std::string script)
            : exprtk::ifunction<T>(0), script(std::move(script)) {}

    inline T operator()() {
        return PythonParser::run(script, {});
    }

private:
    std::string script;
};

#endif //QCALC_SCRIPTFUNCTION_HPP
