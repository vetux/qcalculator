#ifndef QCALC_SCRIPTHANDLER_HPP
#define QCALC_SCRIPTHANDLER_HPP

#include <string>
#include <stdexcept>
#include <vector>

#include "math/arithmetictype.hpp"

struct _object;
typedef _object PyObject;

class ScriptHandler {
public:
    static ArithmeticType run(PyObject *callback, const std::vector<ArithmeticType> &args);
};

#endif //QCALC_SCRIPTHANDLER_HPP
