/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_SYMBOLTABLEUTIL_HPP
#define QCALC_SYMBOLTABLEUTIL_HPP

struct _object;
typedef _object PyObject;

#include "../math/symboltable.hpp"

/**
 * Some helper functions for working with symbol table type on the python side.
 * Instead of creating a new extension type which contains a symbol table instance
 * we will convert between some python object which has the required attributes and
 * our c++ symbol table.
 */
namespace SymbolTableUtil {
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
     * in sym.pycx are present. Which means 4 dictionaries with the names defined in sym.pycx.
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

#endif //QCALC_SYMBOLTABLEUTIL_HPP
