#ifndef QCALC_SCRIPTVARARGFUNCTION_HPP
#define QCALC_SCRIPTVARARGFUNCTION_HPP

#include <string>
#include <cassert>

#include "extern/exprtk.hpp"

#include "calc/pythonparser.hpp"

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
class ScriptVarArgFunction : public exprtk::ivararg_function<T> {
public:
    ScriptVarArgFunction() = default;

    explicit ScriptVarArgFunction(std::string script)
            : script(std::move(script)) {}

    inline T operator()(const std::vector<T> &args) {
        std::vector<double> pyArgs{args.begin(), args.end()};
        return PythonParser::run(script, pyArgs);
    }

private:
    std::string script;
};

#endif //QCALC_SCRIPTVARARGFUNCTION_HPP
