#ifndef QCALC_MODULECOMMON_HPP
#define QCALC_MODULECOMMON_HPP

// Try / Catch macros used at the beginning and end of each function block invoked by the python interpreter.
// This is needed because cpython is a c library which does not handle c++ exceptions,
// instead we have to catch all exceptions and give the error messages back to python,
// which in turn causes python to set the error indicator which we then can retrieve again on the invocation site.
#define MODULE_FUNC_TRY try {
#define MODULE_FUNC_CATCH } catch (const std::exception &e) { PyErr_SetString(PyExc_RuntimeError, e.what()); return PyNull; }

#endif //QCALC_MODULECOMMON_HPP
