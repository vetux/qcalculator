#ifndef QCALC_SCRIPTHANDLER_HPP
#define QCALC_SCRIPTHANDLER_HPP

#include <string>
#include <stdexcept>
#include <vector>

struct _object;
typedef _object PyObject;

class ScriptHandler {
public:
    static double run(PyObject *callback, const std::vector<double> &args);
};

#endif //QCALC_SCRIPTHANDLER_HPP
