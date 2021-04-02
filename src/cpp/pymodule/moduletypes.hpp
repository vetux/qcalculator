#ifndef QCALC_MODULETYPES_HPP
#define QCALC_MODULETYPES_HPP

#define PY_SSIZE_T_CLEAN

#include <Python.h>

#include "calc/symboltable.hpp"

typedef struct {
    PyObject_HEAD
    SymbolTable table;
} PySymbolTable;

#endif //QCALC_MODULETYPES_HPP
