#include "pythoninclude.hpp"

#include "pymodule/presentermodule.hpp"
#include "pymodule/pysymboltable.hpp"
#include "pymodule/modulecommon.hpp"

#include "pyutil.hpp"

#define MODULE_NAME "qc_native_presenter" //The name of the exported module accessible by python

MainPresenter *p = nullptr;

extern "C"
{

PyObject *setSymbolTable(PyObject *self, PyObject *args) {
    MODULE_FUNC_TRY

        PyObject *o;

        if (!PyArg_ParseTuple(args, "O:", &o)) {
            return PyNull;
        }

        SymbolTable t = PySymbolTable::Convert(o);

        PySymbolTable::Cleanup(p->getSymbolTable());

        p->setSymbolTable(t);

        return PyLong_FromLong(0);

    MODULE_FUNC_CATCH
}

PyObject *getSymbolTable(PyObject *self, PyObject *args) {
    MODULE_FUNC_TRY

        if (!PyArg_ParseTuple(args, ":"))
            return PyNull;

        return PySymbolTable::New(p->getSymbolTable());

    MODULE_FUNC_CATCH
}

static PyMethodDef MethodDef[] = {
        {"setSymbolTable", setSymbolTable, METH_VARARGS, "."},
        {"getSymbolTable", getSymbolTable, METH_VARARGS, "."},
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

void PresenterModule::initialize(MainPresenter &pres) {
    p = &pres;
    PyImport_AppendInittab(MODULE_NAME, PyInit);
}