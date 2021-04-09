#include "exprtkmodule.hpp"
#include "modulecommon.hpp"

#include "math/expressionparser.hpp"

#include "cpython/pyutil.hpp"
#include "cpython/pythoninclude.hpp"
#include "cpython/symboltableconverter.hpp"

#define MODULE_NAME "qc_native_exprtk"

extern "C"
{

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

static PyMethodDef MethodDef[] = {
        {"evaluate", evaluate, METH_VARARGS, "."},
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
    return PyModule_Create(&ModuleDef);
}

}

void ExprtkModule::initialize() {
    PyImport_AppendInittab(MODULE_NAME, PyInit);
}
