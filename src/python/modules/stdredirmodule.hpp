/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef QCALC_STDREDIRMODULE_HPP
#define QCALC_STDREDIRMODULE_HPP

#include <functional>
#include <string>

/**
 * When this module is initialized it redirects the std out and std error of the python interpreter to
 * the specified function objects.
 */
namespace StdRedirModule {
    /**
     * Initialize the stdredir python module,
     * this appends logic to the cpython init tab
     * and should therefore be called before initializing cpython.
     *
     * @param stdOut The stream to redirect the python stdout to
     * @param stdErr The stream to redirect the python stderr to
     */
    void initialize();

    void startRedirect(std::function<void(const std::string &)> stdOut,
                       std::function<void(const std::string &)> stdErr);

    void stopRedirect();
}

#endif //QCALC_STDREDIRMODULE_HPP
