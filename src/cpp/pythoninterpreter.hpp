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
    PythonInterpreter() {
        Py_Initialize();
        main = PyImport_AddModule("__main__");
    }

    ~PythonInterpreter() {
        Py_Finalize();
    }

    long double run(const std::string &src, const std::vector<long double> &args) {
        PyObject *globalDictionary = PyModule_GetDict(main);
        PyObject *localDictionary = PyDict_New();

        PyObject *pythonlist = PyList_New(0);

        for (auto arg : args) {
            PyObject *f = PyFloat_FromDouble(arg);
            PyList_Append(pythonlist, f);
        }

        PyDict_SetItemString(localDictionary, "argv", pythonlist);

        PyObject *result = PyRun_String(src.c_str(), Py_file_input, globalDictionary, localDictionary);

        if (result == nullptr) {
            PyErr_Print();
            throw std::runtime_error("Failed to run script");
        } else {
            PyObject *output = PyDict_GetItemString(localDictionary, "outv");
            if (output != nullptr) {
                return PyFloat_AsDouble(output);
            } else {
                return 0;
            }
        }
    }
};

#endif //QT_CALC_PYTHONINTERPRETER_HPP
