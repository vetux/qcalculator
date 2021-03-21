#ifndef QT_CALC_PYTHONINTERPRETER_HPP
#define QT_CALC_PYTHONINTERPRETER_HPP

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <string>
#include <stdexcept>
#include <vector>

class PythonInterpreter {
    PyObject *main;
public:
    PythonInterpreter();

    ~PythonInterpreter();

    long double run(const std::string &src, const std::vector<long double> &args);
};

#endif //QT_CALC_PYTHONINTERPRETER_HPP
