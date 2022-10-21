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

#ifndef QCALC_MODULECOMMON_HPP
#define QCALC_MODULECOMMON_HPP

// Try / Catch macros used at the beginning and end of each function block invoked by the python interpreter.
// This is needed because cpython is a c library which does not handle c++ exceptions,
// instead we have to catch all exceptions and give the error messages back to python,
// which in turn causes python to set the error indicator which we then can retrieve again on the invocation site.
#define MODULE_FUNC_TRY try {
#define MODULE_FUNC_CATCH } catch (const std::exception &e) { PyErr_SetString(PyExc_RuntimeError, e.what()); return NULL; }

#endif //QCALC_MODULECOMMON_HPP
