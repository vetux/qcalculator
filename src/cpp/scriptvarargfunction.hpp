#ifndef QT_CALC_SCRIPTFUNCTION
#define QT_CALC_SCRIPTFUNCTION

#include "pythoninterpreter.hpp"

#include <utility>

#include "extern/exprtk.hpp"

/**
 * A exprtk function which executes a single python script.
 */
template<typename T>
class ScriptVarArgFunction : public exprtk::ivararg_function<T> {
public:
    ScriptVarArgFunction()
            : interpreter(nullptr) {}

    ScriptVarArgFunction(PythonInterpreter &interpreter, std::string script)
            : interpreter(&interpreter), script(std::move(script)) {}

    inline T operator()(const std::vector<T> &args) {
        assert(interpreter != nullptr);
        return interpreter->run(script, args);
    }

private:
    PythonInterpreter *interpreter;
    std::string script;
};

#endif //QT_CALC_SCRIPTFUNCTION
