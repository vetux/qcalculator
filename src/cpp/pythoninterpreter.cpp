#include "pythoninterpreter.hpp"

PythonInterpreter::PythonInterpreter() {
    Py_Initialize();
    main = PyImport_AddModule("__main__");
}

PythonInterpreter::~PythonInterpreter() {
    Py_Finalize();
}

ValueType PythonInterpreter::run(const std::string &src, const std::vector<ValueType> &args) {
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
        /*
        //The documentation does not give clear insight as to what PyErr_Fetch does so i will not risk a segfault because of bad documentation.
        PyObject *pType, *pValue, *pTraceback;
        PyErr_Fetch(&pType, &pValue, &pTraceback);

        PyObject *pTypeStr = PyObject_Str(pType);
        PyObject *pValueStr = PyObject_Str(pValue);
        PyObject *pTracebackStr = PyObject_Str(pTraceback);

        const char *pErrorType = PyUnicode_AsUTF8(pTypeStr);
        const char *pErrorValue = PyUnicode_AsUTF8(pValueStr);
        const char *pErrorTraceback = PyUnicode_AsUTF8(pTracebackStr);

        std::string error = "Failed to run script, Error{ Type: ";
        error += pErrorType;
        error += ", Value: ";
        error += pErrorValue;
        error += ", Traceback: ";
        error += pErrorTraceback;
        error += " }";

        throw std::runtime_error(error);
        */
        PyErr_Print();
        throw std::runtime_error("Failed to run script, See standard error for a detailed error description");
    } else {
        PyObject *output = PyDict_GetItemString(localDictionary, "outv");
        if (output != nullptr) {
            return PyFloat_AsDouble(output);
        } else {
            return 0;
        }
    }
}