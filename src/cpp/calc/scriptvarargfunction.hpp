#ifndef QCALC_SCRIPTVARARGFUNCTION_HPP
#define QCALC_SCRIPTVARARGFUNCTION_HPP

#include <string>

#include "extern/exprtk.hpp"

#include "calc/pythonparser.hpp"

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
class ScriptVarArgFunction : public exprtk::ivararg_function<T> {
public:
    ScriptVarArgFunction()
            : pythonParser(nullptr) {}

    ScriptVarArgFunction(PythonParser &pythonParser, std::string script)
            : pythonParser(&pythonParser), script(std::move(script)) {}

    inline T operator()(const std::vector<T> &args) {
        assert(pythonParser != nullptr);
        std::vector<double> pyArgs{args.begin(), args.end()};
        return pythonParser->run(script, pyArgs);
    }

private:
    PythonParser *pythonParser;
    std::string script;
};

#endif //QCALC_SCRIPTVARARGFUNCTION_HPP
