/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
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

#ifndef QCALC_MPREALMODULE_HPP
#define QCALC_MPREALMODULE_HPP

namespace MprealModule {
    /**
     * Initialize the mpreal python module,
     * this appends logic to the cpython init tab
     * and should therefore be called before initializing cpython.
     *
     * @param globalTable
     * @param tableChangeCallback
     */
    void initialize();
}

#endif //QCALC_MPREALMODULE_HPP
