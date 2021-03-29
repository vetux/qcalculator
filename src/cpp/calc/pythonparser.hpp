#ifndef QCALC_PYTHONPARSER_HPP
#define QCALC_PYTHONPARSER_HPP

#include <string>
#include <stdexcept>
#include <vector>

struct _object;
typedef _object PyObject;

/**
 * The parser object for script functions.
 * Parses a python script in string form, passes the arguments to the script int the python list "argv" and returns the value of the python float "outv"
 */
class PythonParser {
public:
    PythonParser();

    ~PythonParser();

    double run(const std::string &src, const std::vector<double> &args);

private:
    PyObject *qcModule;
};

#endif //QCALC_PYTHONPARSER_HPP
