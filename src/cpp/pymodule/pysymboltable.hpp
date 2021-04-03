#ifndef QCALC_PYSYMBOLTABLE_HPP
#define QCALC_PYSYMBOLTABLE_HPP

struct _object;
typedef _object PyObject;

#include "calc/symboltable.hpp"

/**
 * Some helper functions for working with symbol table type on the python side.
 * Instead of creating a new extension type which contains a symbol table instance
 * we will convert between some python object which has the required attributes and
 * our c++ symbol table.
 */
namespace PySymbolTable {
    /**
     * Throws on error and does NOT touch python errors in any way.
     *
     * References to script callbacks are incremented.
     *
     * @param table
     * @return
     */
    PyObject *New(const SymbolTable &table);

    /**
     * Throws on error and does NOT touch python errors in any way.
     *
     * Does extensive type checking on the relevant attributes of the object.
     *
     * The passed object may be any class instance as long as the attributes specified
     * in sym.py are present. Which means 4 dictionaries with the names defined in sym.py.
     *
     * All keys have to be unicode strings.
     *
     * The variable and constant values have to be float or long.
     *
     * The function and script values can be any class instance as long as the corresponding
     * attributes are present.
     *
     * References to script callbacks are incremented.
     *
     * @param o
     * @return
     */
    SymbolTable Convert(PyObject *o);

    /**
     * Decrements all script callback object reference counts in the passed tables script definitions,
     * and returns the new table with the scripts cleared.
     *
     * @param table
     */
    SymbolTable Cleanup(const SymbolTable &table);
}

#endif //QCALC_PYSYMBOLTABLE_HPP
