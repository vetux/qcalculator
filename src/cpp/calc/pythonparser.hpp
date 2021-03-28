#ifndef QCALC_PYTHONPARSER_HPP
#define QCALC_PYTHONPARSER_HPP

#define PY_SSIZE_T_CLEAN

#include <Python.h> //TODO: Move python include into source file

#include <string>
#include <stdexcept>
#include <vector>

/**
 * The parser object for script functions.
 * Parses a python script in string form, passes the arguments to the script int the python list "argv" and returns the value of the python float "outv"
 */
class PythonParser {
    PyObject *main;
public:
    PythonParser();

    ~PythonParser();

    double run(const std::string &src, const std::vector<double> &args);
};

#endif //QCALC_PYTHONPARSER_HPP
