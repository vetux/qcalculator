#include "pymodule/exprtkmodule.hpp"
#include "pymodule/modulecommon.hpp"

#include "calc/expressionparser.hpp"

#include "pyutil.hpp"
#include "pythoninclude.hpp"
#include "pymodule/pysymboltable.hpp"

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

        SymbolTable symTable = PySymbolTable::Convert(pySymTable);

        ArithmeticType value = ExpressionParser::evaluate(expression, symTable);

        PyObject *ret = PyTuple_New(2);

        PyTuple_SetItem(ret, 0, PyFloat_FromDouble(value.toDouble()));
        PyTuple_SetItem(ret, 1, PySymbolTable::New(symTable));

        PySymbolTable::Cleanup(symTable);

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
