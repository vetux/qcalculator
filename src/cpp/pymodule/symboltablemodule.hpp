#ifndef QCALC_SYMBOLTABLEMODULE_HPP
#define QCALC_SYMBOLTABLEMODULE_HPP

#include "gui/mainpresenter.hpp"

struct _object;
typedef _object PyObject;

/**
 * A module which allows manipulation of the symbol table in a presenter instance.
 */
class SymbolTableModule {
public:
    /**
     * Register a new native python module and link up the static member functions of this class to the module.
     * Should be called before initializing python.
     *
     * @param view The target view
     * @param presenter The target presenter
     */
    static void initialize(MainPresenter &presenter);


    static PyObject *remove(PyObject *self, PyObject *args);


    static PyObject *getVariableNames(PyObject *self, PyObject *args);

    static PyObject *getVariable(PyObject *self, PyObject *args);

    static PyObject *setVariable(PyObject *self, PyObject *args);


    static PyObject *getConstantNames(PyObject *self, PyObject *args);

    static PyObject *getConstant(PyObject *self, PyObject *args);

    static PyObject *setConstant(PyObject *self, PyObject *args);


    static PyObject *getFunctionNames(PyObject *self, PyObject *args);

    static PyObject *getFunctionExpression(PyObject *self, PyObject *args);

    static PyObject *getFunctionArgumentNames(PyObject *self, PyObject *args);

    static PyObject *setFunction(PyObject *self, PyObject *args);


    static PyObject *getScriptFunctionNames(PyObject *self, PyObject *args);

    static PyObject *getScriptFunctionCallback(PyObject *self, PyObject *args);

    static PyObject *getScriptFunctionEnableArguments(PyObject *self, PyObject *args);

    static PyObject *setScriptFunction(PyObject *self, PyObject *args);

private:
    static MainPresenter *presenter;
};

#endif //QCALC_SYMBOLTABLEMODULE_HPP
