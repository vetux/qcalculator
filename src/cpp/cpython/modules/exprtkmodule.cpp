#include "cpython/modules/exprtkmodule.hpp"

#include "cpython/pyutil.hpp"
#include "cpython/pythoninclude.hpp"
#include "cpython/symboltableconverter.hpp"
#include "cpython/modules/modulecommon.hpp"
#include "cpython/types/pympreal.hpp"

#include "math/expressionparser.hpp"

#define MODULE_NAME "qc_native_exprtk"

SymbolTable *globalSymbolTable = nullptr;

PyObject *evaluate(PyObject *self, PyObject *args) {
    MODULE_FUNC_TRY

        PyObject *pyExpression;
        PyObject *pySymTable;

        if (!PyArg_ParseTuple(args, "OO:", &pyExpression, &pySymTable)) {
            return PyNull;
        }

        const char *expression = PyUnicode_AsUTF8(pyExpression);
        if (expression == PyNull) {
            return PyNull;
        }

        SymbolTable symTable = SymbolTableConverter::Convert(pySymTable);

        ArithmeticType value = ExpressionParser::evaluate(expression, symTable);

        PyObject *ret = PyTuple_New(2);

        PyTuple_SetItem(ret, 0, PyFloat_FromDouble(value.toDouble()));
        PyTuple_SetItem(ret, 1, SymbolTableConverter::New(symTable));

        SymbolTableConverter::Cleanup(symTable);

        return ret;

    MODULE_FUNC_CATCH
}

PyObject *get_global_symtable(PyObject *self, PyObject *args) {
    return SymbolTableConverter::New(*globalSymbolTable);
}

PyObject *set_global_symtable(PyObject *self, PyObject *args) {
    PyObject *pysym = PyNull;
    if (!PyArg_ParseTuple(args, "O:", &pysym)) {
        return PyNull;
    }
    *globalSymbolTable = SymbolTableConverter::Convert(pysym);
    return PyLong_FromLong(0);
}

static PyMethodDef MethodDef[] = {
        {"evaluate",            evaluate,            METH_VARARGS, "."},
        {"get_global_symtable", get_global_symtable, METH_NOARGS,  "."},
        {"set_global_symtable", set_global_symtable, METH_VARARGS, "."},
        {PyNull, PyNull, 0, PyNull}
};

static PyModuleDef ModuleDef = {
        PyModuleDef_HEAD_INIT,
        MODULE_NAME,
        PyNull,
        -1,
        MethodDef,
        PyNull, PyNull, PyNull, PyNull
};

static PyObject *PyInit() {
    PyObject *m;

    m = PyModule_Create(&ModuleDef);
    if (m == PyNull)
        return PyNull;

    if (!PyMpReal_Initialize(m)) {
        Py_DECREF(m);
        return PyNull;
    }

    return m;
}

void ExprtkModule::initialize(SymbolTable &gs) {
    globalSymbolTable = &gs;
    PyImport_AppendInittab(MODULE_NAME, PyInit);
}
