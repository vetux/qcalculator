#ifndef QT_CALC_PYTHONINTERPRETER_HPP
#define QT_CALC_PYTHONINTERPRETER_HPP

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include <string>
#include <stdexcept>
#include <vector>

#include "valuetype.hpp"

class PythonInterpreter {
    PyObject *main;
public:
    PythonInterpreter();

    ~PythonInterpreter();

    ValueType run(const std::string &src, const std::vector<ValueType> &args);
};

#endif //QT_CALC_PYTHONINTERPRETER_HPP
