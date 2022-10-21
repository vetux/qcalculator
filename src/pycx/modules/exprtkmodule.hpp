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

#ifndef QCALC_EXPRTKMODULE_HPP
#define QCALC_EXPRTKMODULE_HPP

#include <functional>

#include "math/symboltable.hpp"

namespace ExprtkModule {
    /**
     * Initialize the exprtk python module,
     * this appends logic to the cpython init tab
     * and should therefore be called before initializing cpython.
     *
     * Dependency on mpreal module, initialize mpreal module beforehand.
     */
    void initialize();

    void setGlobalTable(SymbolTable &globalTable, std::function<void()> tableChangeCallback);
}

#endif //QCALC_EXPRTKMODULE_HPP
